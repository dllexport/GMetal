#include <RenderPass.h>

RenderPass::~RenderPass()
{
	vkDestroyDescriptorPool(context->GetVkDevice(), this->descriptorPool, nullptr);
	vkDestroyRenderPass(context->GetVkDevice(), this->renderPass, nullptr);
}
