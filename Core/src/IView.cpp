//
// Created by Mario on 2023/1/18.
//

#include <IView.h>

IView::IView(boost::intrusive_ptr<VulkanContext>& context, VkSurfaceKHR surface) : vulkanContext(context), surface(surface)
{

}

IView::~IView()
{
	vkDestroySurfaceKHR(vulkanContext->GetVkInstance(), surface, nullptr);
}
