#include <RenderPassBuilder.h>
#include <unordered_map>

RenderPassBuilder::RenderPassBuilder(IntrusivePtr<VulkanContext>& context)
{
	this->context = context;
}

RenderPassBuilder& RenderPassBuilder::SetAttachments(std::vector<VkAttachmentDescription> attachments)

{
	this->attachments = attachments;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::AddSubPass(VkPipelineBindPoint bindPoint, std::vector<VkAttachmentReference>&& colorRefs, VkAttachmentReference depthStencilRef, std::vector<VkAttachmentReference>&& inputRefs, std::vector<uint32_t>&& reserveRefs, VkAttachmentReference resolveRef)
{
	SubPassPair pair = {};
	pair.colorRefs = colorRefs;
	pair.depthStencilRef = depthStencilRef;
	pair.inputRefs = inputRefs;
	pair.reserveRefs = reserveRefs;
	pair.resolveRef = resolveRef;
	this->subPassPairs.push_back(pair);
	return *this;
}

RenderPassBuilder& RenderPassBuilder::AddDependency(VkSubpassDependency dependency)
{
	this->dependencies.push_back(dependency);
	return *this;
}

void RenderPassBuilder::BuildDescriptorPool()
{
	std::unordered_map<VkDescriptorType, uint32_t> descriptorCounterMap;
	for (int i = 0; i < pipelines.size(); i++)
	{
		auto& pipeline = pipelines[i];
		for (auto bindings : pipeline->descriptorSetLayoutBindings) {
			for (auto binding : bindings) {
				descriptorCounterMap[binding.descriptorType] += binding.descriptorCount;
			}
		}
	}

	std::vector<VkDescriptorPoolSize> poolSizes;
	for (auto& [k, v] : descriptorCounterMap)
	{
		poolSizes.emplace_back(k, v);
	}

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = poolSizes.size();
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	// TODO, change this
	descriptorPoolInfo.maxSets = 8;

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	vkCreateDescriptorPool(context->GetVkDevice(), &descriptorPoolInfo, nullptr, &descriptorPool);

	this->renderPass->descriptorPool = descriptorPool;
}

void RenderPassBuilder::BuildSubPass()
{
	for (auto& pair : subPassPairs)
	{
		VkSubpassDescription sd = {};
		sd.pipelineBindPoint = pair.bindPoint;
		sd.colorAttachmentCount = pair.colorRefs.size();
		sd.pColorAttachments = pair.colorRefs.data();
		sd.pDepthStencilAttachment = &pair.depthStencilRef;
		sd.inputAttachmentCount = pair.inputRefs.size();
		sd.pInputAttachments = pair.inputRefs.data();
		sd.preserveAttachmentCount = pair.reserveRefs.size();
		sd.pPreserveAttachments = pair.reserveRefs.data();
		if (pair.resolveRef.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
			sd.pResolveAttachments = nullptr;
		}
		else {
			sd.pResolveAttachments = &pair.resolveRef;
		}
		subPassDescriptions.push_back(sd);
	}

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subPassDescriptions.size());
	renderPassInfo.pSubpasses = subPassDescriptions.data();
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	VkRenderPass pass;
	vkCreateRenderPass(context->GetVkDevice(), &renderPassInfo, nullptr, &pass);

	renderPass->renderPass = pass;
}

IntrusivePtr<RenderPass> RenderPassBuilder::Build()
{
	this->renderPass = gmetal::make_intrusive<RenderPass>();
	this->renderPass->context = this->context;
	BuildSubPass();
	BuildDescriptorPool();
	return renderPass;
}