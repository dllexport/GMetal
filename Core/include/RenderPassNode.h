#pragma once

#include <FrameGraph.h>
#include <FrameGraphNode.h>
#include <IntrusivePtr.h>
#include <Pipeline.h>
#include <PipelineBuilder.h>
#include <unordered_map>

class RenderPassNode : public FrameGraphNode {
public:
    struct SlotOp {
        uint32_t index;
        bool samplerRequired;
    };

    RenderPassNode() {}
    virtual ~RenderPassNode() {}

    template <class T> void Read(IntrusivePtr<T>& node) { this->fg->Link(node, this); }

    template <class T> void Read(IntrusivePtr<T>& node, uint32_t slot, bool sample = false) {
        this->fg->Link(node, this);
        slotIndexMap[node] = {slot, sample};
    }

    template <class T> void Write(IntrusivePtr<T>& node) { this->fg->Link(this, node); }

private:
    friend class FrameGraph;
    std::function<IntrusivePtr<Pipeline>(PipelineBuilder& builder)> pipelineSetupFn;
    std::function<void()> pipelineExecuteFn;
    std::unordered_map<IntrusivePtr<FrameGraphNode>, SlotOp> slotIndexMap;

public:
    void Setup(decltype(pipelineSetupFn)&& fn) { pipelineSetupFn = fn; }
    void Execute(decltype(pipelineExecuteFn)&& fn) { pipelineExecuteFn = fn; }
};
