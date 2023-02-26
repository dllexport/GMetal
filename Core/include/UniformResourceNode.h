#pragma once

#include <FrameGraph.h>
#include <FrameGraphNode.h>
#include <GeneralBuffer.h>
#include <ResourceNode.h>
#include <ResourceNodeVisitor.h>
#include <vulkan/vulkan.h>

class UniformResourceNode : public ResourceNode {
public:
    UniformResourceNode();
    virtual ~UniformResourceNode();

    template<class UniformType>
    void SetValue(UniformType value)
    { 
        if (!buffer) {
            buffer = std::make_unique<char[]>(sizeof(UniformType));
            length = sizeof(UniformType);
        }
        *(UniformType*)buffer.get() = value;
    }

    template <class UniformType> UniformType& GetValue() 
    { 
        return *(UniformType*)buffer.get();
    }

    IntrusivePtr<GeneralBuffer>& GetGeneralBuffer() { 
        return gpuBuffer;
    }

    virtual void Accept(ResourceNodeVisitor* visitor);

    virtual void Resolve(VkExtent3D extend);

private:
    friend class FrameGraph;
    IntrusivePtr<GeneralBuffer> gpuBuffer;
    std::unique_ptr<char[]> buffer;
    uint32_t length = 0;
};
