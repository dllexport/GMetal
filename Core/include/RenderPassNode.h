#pragma once

#include <FrameGraph.h>
#include <FrameGraphNode.h>
#include <IntrusivePtr.h>

class RenderPassNode : public FrameGraphNode
{
public:
	RenderPassNode() {}
	virtual ~RenderPassNode() {}

	template<class T>
	void Read(IntrusivePtr<T>& node) {
		this->fg->Link(node, this);
	}

	template<class T>
	void Write(IntrusivePtr<T>& node) {
		this->fg->Link(this, node);
	}

private:

};
