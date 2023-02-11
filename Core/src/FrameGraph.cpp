#include <FrameGraph.h>

#include <queue>
#include <unordered_map>

#include <ImageResourceNode.h>
#include <RenderPassNode.h>

FrameGraph::FrameGraph()
{

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
	std::vector<VkAttachmentDescription> vads;
	for (auto& imageNode : imageNodes)
	{
		if (imageNode->refCount > 0) {
			auto imageResourceNode = static_cast<ImageResourceNode*>(imageNode.get());
			vads.push_back(imageResourceNode->vad);
		}
	}

	std::vector<VkSubpassDescription> vsds;
	std::vector<VkAttachmentReference> vafs;

	for (int i = 0; i < passNodes.size(); i++)
	{
		auto renderPassNode = static_cast<RenderPassNode*>(passNodes[i].get());
		auto& ins = this->nodesInMap[renderPassNode];
		auto& outs = this->nodesOutMap[renderPassNode];

		for (uint32_t j = 0; j < outs.size(); j++)
		{
			VkAttachmentReference vaf{ j, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
			vafs.push_back(vaf);
		}

		VkSubpassDescription vsd;
		vsd.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vsd.colorAttachmentCount = outs.size();
		vsd.pColorAttachments = vafs.data();
		vsd.pDepthStencilAttachment = &depthReference;
		vsds.push_back(vsd);
	}
}

void FrameGraph::Compile()
{
	Cull();
	TopoSort();
	BuildRenderPass();
}