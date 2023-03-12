#pragma once

#include <FrameGraph.h>
#include <FrameGraphNode.h>
#include <IntrusivePtr.h>
#include <Pipeline.h>
#include <PipelineBuilder.h>
#include <unordered_map>

#include <ImageResourceNode.h>

class RenderPassNode : public FrameGraphNode {
public:

    RenderPassNode() {}
    virtual ~RenderPassNode() {}

    // write color attachments
    template <class T> void Write(IntrusivePtr<T>& node, uint32_t binding) { 
        this->fg->Link(this, node);
        colorImageNodes[node] = binding;
    }

    // read input attachments
    template <class T> void Read(IntrusivePtr<T>& node, uint32_t binding) {
        this->fg->Link(node, this);
        if constexpr (std::is_same_v<T, ImageResourceNode>) {
            inputImageNodes[node] = binding;
        } else if constexpr (std::is_base_of_v<UniformResourceNode, T>) {
            inputUniformNodes[node] = binding;
        } else {
            throw std::runtime_error("unknown resource type");
        }
    }

    // resolve attachment
    template <class T> void Resolve(IntrusivePtr<T>& node) {
        this->fg->Link(node, this);
        this->resolveImageNode = node;
    }

    // depth attachment, read op: depth -> pass
    template <class T> void DepthStencilReadWrite(IntrusivePtr<T>& node) { 
        this->fg->Link(node, this);
        this->depthStencilImageNode = node;
    }

    // preserve read op
    template <class T> void Preserve(IntrusivePtr<T>& node) {
        this->fg->Link(node, this);
        this->preserveImageNodes.push_back(node);
    }

private:
    friend class FrameGraph;
    std::function<IntrusivePtr<Pipeline>(PipelineBuilder& builder)> pipelineSetupFn;
    std::function<void(VkCommandBuffer)> pipelineExecuteFn;

    std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t> inputUniformNodes;
    std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t> inputImageNodes;
    std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t> colorImageNodes;
    IntrusivePtr<FrameGraphNode> resolveImageNode;
    std::vector<IntrusivePtr<FrameGraphNode>> preserveImageNodes;
    IntrusivePtr<FrameGraphNode> depthStencilImageNode;

public:
    // setup pipeline state via PipelineBuilder
    // 1. pipeline layout (AddDescriptorSetLayoutBinding)
    // 2. shader source
    // 3. vertex attribute, rasterizer, etc.
    void Setup(decltype(pipelineSetupFn)&& fn) { pipelineSetupFn = fn; }
    void Execute(decltype(pipelineExecuteFn)&& fn) { pipelineExecuteFn = fn; }
};
