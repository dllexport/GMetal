#include <Image.h>

Image::Image(IntrusivePtr<VulkanContext>& context) : context(context)
{

}

Image::~Image()
{
	if (!externResource) {
		vmaDestroyImage(context->GetVmaAllocator(), this->image, this->allocation);
	}
}

void Image::Assign(VkImage image, VkFormat format)
{
	this->image = image;
	this->vici.format = format;
	externResource = true;
}

VkFormat Image::GetFormat()
{
	return this->vici.format;
}