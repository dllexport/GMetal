#pragma once

#include <vulkan/vulkan.h>

#include <Drawable.h>
#include <IntrusivePtr.h>
#include <VulkanContext.h>

class Pipeline : public IntrusiveUnsafeCounter<Pipeline> {
public:
    Pipeline();
    ~Pipeline();

    enum class Type { GRAPHICS, COMPUTE };

    VkDescriptorSet& GetDescriptorSet(uint32_t frameIndex, uint32_t setIndex) {
        return descriptorSets[frameIndex][setIndex];
    }

    std::vector<std::vector<VkDescriptorSetLayoutBinding>>& GetDescriptorSetLayoutBindings() {
        return descriptorSetLayoutBindings;
    }

    std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() { return descriptorSetLayouts; }

    VkPipeline& GetPipeline() { return pipeline; }
    VkPipelineLayout& GetPipelineLayout() { return pipelineLayout; }

    IntrusivePtr<VulkanContext>& Context() { return context; }

private:
    friend class FrameGraph;
    friend class PipelineBuilder;
    friend class RenderPassBuilder;
    friend class DescriptorPoolBuilder;
    friend class Renderer;

    IntrusivePtr<VulkanContext> context;
    Type type;
    VkPipeline pipeline;
    std::vector<VkShaderModule> shaderModules;
    VkPipelineLayout pipelineLayout;
    // pipeline could have multiple descriptor set
    std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    // descriptorSet per pipeline per frame
    std::vector<std::vector<VkDescriptorSet>> descriptorSets;
};