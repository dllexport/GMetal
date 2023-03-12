#include <DescriptorPoolBuilder.h>


DescriptorPoolBuilder::DescriptorPoolBuilder(IntrusivePtr<VulkanContext>& context) : context(context)
{

}

DescriptorPoolBuilder& DescriptorPoolBuilder::AddPipeline(IntrusivePtr<Pipeline>& pipeline)
{
	this->pipelines.push_back(pipeline);
	return *this;
}

DescriptorPoolBuilder& DescriptorPoolBuilder::SetPipelines(std::vector<IntrusivePtr<Pipeline>> pipelines)
{
	this->pipelines = pipelines;
	return *this;
}

DescriptorPoolBuilder& DescriptorPoolBuilder::SetRenderPass(IntrusivePtr<RenderPass>& renderPass)
{
	this->renderPass = renderPass;
	return *this;
}

DescriptorPoolBuilder& DescriptorPoolBuilder::SetSwapChainSize(uint32_t size)
{
    this->swapChainSize = size;
    return *this;
}

void DescriptorPoolBuilder::BuildDescriptorPool()
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
		poolSizes.push_back({k, v * swapChainSize});
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

void DescriptorPoolBuilder::Build()
{
	BuildDescriptorPool();
}