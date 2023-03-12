#include <ImageView.h>

ImageView::ImageView(IntrusivePtr<VulkanContext>& context) : context(context)
{

}

ImageView::~ImageView()
{ 
	vkDestroyImageView(context->GetVkDevice(), this->imageView, nullptr);
}
