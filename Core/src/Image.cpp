#include <Image.h>

void Image::Assign(VkImage image, VkFormat format)
{
	this->image = image;
	this->vici.format = format;
}


VkFormat Image::GetFormat()
{
	return this->vici.format;
}