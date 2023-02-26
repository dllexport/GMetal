#include <GeneralBuffer.h>

GeneralBuffer::GeneralBuffer(IntrusivePtr<VulkanContext>& context) : context(context) {}

GeneralBuffer::~GeneralBuffer() {
    vmaDestroyBuffer(context->GetVmaAllocator(), this->buffer, this->allocation);
}
