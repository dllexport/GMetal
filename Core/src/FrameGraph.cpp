#include <FrameGraph.h>

#include <queue>
#include <unordered_map>

#include <ImageResourceNode.h>
#include <RenderPassNode.h>
#include <RenderPassBuilder.h>
#include <PipelineBuilder.h>
#include <ImageBuilder.h>

FrameGraph::FrameGraph(IntrusivePtr<VulkanContext>& context) : context(context)
{

}

FrameGraph::~FrameGraph()
{
	spdlog::info("die");
}

void FrameGraph::Link(IntrusivePtr<FrameGraphNode>&& from, IntrusivePtr<FrameGraphNode>&& to)
{
	nodesInMap[to].push_back(from);
	nodesOutMap[from].push_back(to);
	from->refCount++;
}

void FrameGraph::TopoSort()
{
	levelResult.resize(passNodes.size() + imageNodes.size());

	// nodes in degree counter map
	std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t> counterMap;
	std::queue<IntrusivePtr<FrameGraphNode>> sortQueue;
	for (auto& [k, v] : nodesInMap)
	{
		counterMap[k] = v.size();
		if (v.empty()) {
			sortQueue.push(k);
		}
	}

	uint32_t levelCounter = sortQueue.size();
	uint32_t currentLevel = 0;
	while (!sortQueue.empty())
	{
		auto front = sortQueue.front();
		sortQueue.pop();
		levelCounter--;
		
		// only valid node get pushed in result
		if (front->refCount != 0) {
			levelResult[currentLevel].push_back(front);
		}

		auto& outs = nodesOutMap[front];
		for (auto& out : outs)
		{
			counterMap[out]--;
			if (counterMap[out] == 0) {
				sortQueue.push(out);
			}
		}

		if (!levelCounter) {
			levelCounter = sortQueue.size();
			currentLevel++;
		}
	}

	levelResult.resize(currentLevel);

#ifndef NDEBUG
	for (int i = 0; i < levelResult.size(); i++) {
		spdlog::info("lv: {}", i);
		for (auto v : levelResult[i]) {
			spdlog::info("	name: {}", v->name.c_str());
		}
	}

	for (auto [k, v] : counterMap) {
		spdlog::info("{} {}", k->name.c_str(), v);
		if (v != 0) {
			throw std::runtime_error("Cyclic graph");
		}
	}
#endif

}

void FrameGraph::Cull()
{
	for (auto& [k, v] : this->nodesOutMap)
	{
		if (v.empty() && k->refCount == 0) {
			TraceBack(k);
		}
	}

	printNode();
}

void FrameGraph::TraceBack(const IntrusivePtr<FrameGraphNode>& node)
{
	if (node->refCount > 0) {
		node->refCount--;
	}
	auto froms = this->nodesInMap[node];
	for (auto& from : froms)
	{
		TraceBack(from);
	}
}

void FrameGraph::BuildRenderPass()
{
	auto renderPassBuilder = RenderPassBuilder(context);

	std::vector<VkAttachmentDescription> vads;
	// imageNode -> index
	std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t> vadsMap;
	std::unordered_map<IntrusivePtr<FrameGraphNode>, std::vector<VkPipelineColorBlendAttachmentState>> passColorAttachmentBlendStates;

	std::vector<VkClearValue> attachmentClearValues;

	for (uint32_t i = 0; i < imageNodes.size(); i++)
	{
		auto imageResourceNode = static_cast<ImageResourceNode*>(imageNodes[i].get());
		vadsMap[imageResourceNode] = i;
		vads.push_back(imageResourceNode->vad);
		attachmentClearValues.push_back(imageResourceNode->clearValue);
	}

	renderPassBuilder.SetClearValues(attachmentClearValues);
	renderPassBuilder.SetAttachments(vads);

	for (uint32_t i = 0; i < passNodes.size(); i++)
	{
		std::vector<VkAttachmentReference> colorRefs;
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendStates;
		std::vector<VkAttachmentReference> inputRefs;
		std::vector<VkAttachmentReference> depthRef;

		auto renderPassNode = static_cast<RenderPassNode*>(passNodes[i].get());
		auto& ins = this->nodesInMap[renderPassNode];
		auto& outs = this->nodesOutMap[renderPassNode];

		bool haveSwapChainAttachment = false;
		// record color refs
		for (uint32_t j = 0; j < outs.size(); j++)
		{
 			auto imageResourceNode = static_cast<ImageResourceNode*>(outs[j].get());
			if (imageResourceNode->isDepthStencil) {
				depthRef.push_back({ vadsMap[imageResourceNode], VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL });
				continue;
			}
			if (imageResourceNode->isSwapChainImage) {
				haveSwapChainAttachment = true;
			}
			colorRefs.push_back({ vadsMap[imageResourceNode], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
			passColorAttachmentBlendStates[renderPassNode].push_back(imageResourceNode->blendState);
		}


		// record input refs
		for (uint32_t j = 0; j < ins.size(); j++)
		{
			auto imageResourceNode = static_cast<ImageResourceNode*>(ins[j].get());
			inputRefs.push_back({ vadsMap[imageResourceNode], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
		}
		renderPassBuilder.AddSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS, std::move(colorRefs), std::move(depthRef), std::move(inputRefs));
		
		bool depRequired = false;
		VkSubpassDependency dep = {};
		if (haveSwapChainAttachment) {
			VkSubpassDependency dep = {};
			dep.srcSubpass = VK_SUBPASS_EXTERNAL;
			dep.dstSubpass = i;
			dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dep.srcAccessMask = 0;
			dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			renderPassBuilder.AddDependency(dep);
		}

		if (ins.size() > 0 && outs.size() > 0) {
			dep.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dep.dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dep.srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dep.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
			depRequired = true;
		}

		dep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		if (depRequired)
		{
			dep.srcSubpass = i - 1;
			dep.dstSubpass = i;
			renderPassBuilder.AddDependency(dep);
		}
	}

	this->renderPass = renderPassBuilder.Build();

	// build pipelines
	for (int i = 0; i < passNodes.size(); i++)
	{
		auto rpNode = static_cast<RenderPassNode*>(passNodes[i].get());
		if (!rpNode->pipelineSetupFn) {
			continue;
			throw std::runtime_error("pipelineSetupFn required");
		}

		auto builder = PipelineBuilder(context).SetRenderPass(renderPass, i)
			.SetBlendAttachmentState(std::move(passColorAttachmentBlendStates[rpNode]))
			.SetRenderPass(renderPass, i);
		auto pipeline = rpNode->pipelineSetupFn(builder);
		renderPass->GetPipelines().push_back(pipeline);
	}
}

void FrameGraph::Compile()
{
	Cull();
	TopoSort();
	ResolveResource();
}

void FrameGraph::ResolveResource(VkExtent2D extent)
{
	for (auto& imageNode : imageNodes)
	{
		auto imageResourceNode = static_cast<ImageResourceNode*>(imageNode.get());
		if (imageResourceNode->image) {
			continue;
		}

		VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		if (imageResourceNode->isDepthStencil)
		{
			imageUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		imageResourceNode->image = ImageBuilder(context)
			.SetBasic(VK_IMAGE_TYPE_2D, imageResourceNode->vad.format, VkExtent3D{ extent.height, extent.width, 1 }, imageUsage)
			.Build();
	}
}


void FrameGraph::Execute()
{

}
