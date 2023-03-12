#include <Pipeline.h>

Pipeline::Pipeline() 
{

}

Pipeline::~Pipeline()
{
	for (auto& shaderModule : shaderModules)
	{
		vkDestroyShaderModule(context->GetVkDevice(), shaderModule, nullptr);
	}

	for (auto& descriptorSetLayout : descriptorSetLayouts)
	{
		vkDestroyDescriptorSetLayout(context->GetVkDevice(), descriptorSetLayout, nullptr);
	}
	vkDestroyPipelineLayout(context->GetVkDevice(), pipelineLayout, nullptr);
	vkDestroyPipeline(context->GetVkDevice(), pipeline, nullptr);
}
