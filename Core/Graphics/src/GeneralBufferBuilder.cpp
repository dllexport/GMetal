#include <GeneralBufferBuilder.h>

GeneralBufferBuilder::GeneralBufferBuilder(IntrusivePtr<VulkanContext>& context) : context(context) {
    vbci = {};
    vbci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    vaci = {};
    vaci.usage = VMA_MEMORY_USAGE_AUTO;
    vaci.priority = 1.0f;
}

GeneralBufferBuilder& GeneralBufferBuilder::SetBasic(VkDeviceSize size, VkBufferUsageFlags usage) {
    vbci.size = size;
    vbci.usage = usage;
    return *this;
}

GeneralBufferBuilder& GeneralBufferBuilder::SetSharingMode(VkSharingMode mode) {
    vbci.sharingMode = mode;
    return *this;
}

GeneralBufferBuilder& GeneralBufferBuilder::SetQueueFamilies(std::vector<uint32_t> queueFamilies) {
    this->queueFamilies = queueFamilies;
    return *this;
}

GeneralBufferBuilder& GeneralBufferBuilder::SetAllocationInfo(VmaMemoryUsage usage, VmaAllocationCreateFlags flags) {
    vaci.usage = usage;
    vaci.flags = flags;
    vaci.priority = 1.0f;
    return *this;
}

IntrusivePtr<GeneralBuffer> GeneralBufferBuilder::Build() {
    auto buffer = gmetal::make_intrusive<GeneralBuffer>(context);

    vmaCreateBuffer(context->GetVmaAllocator(),
                    &vbci,
                    &vaci,
                    &buffer->buffer,
                    &buffer->allocation,
                    nullptr);

    return buffer;
}