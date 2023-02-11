#pragma once

#include <string>
#include <IntrusivePtr.h>

class FrameGraph;
class FrameGraphNode : public IntrusiveCounter<FrameGraphNode>
{
public:
	FrameGraphNode(IntrusivePtr<FrameGraph> fg = nullptr);
	virtual ~FrameGraphNode() {};

	// retain the node so it won't be culled
	void Retain() { refCount++; }
	bool Valid() { return refCount > 0; }

protected:
	friend class FrameGraph;
	IntrusivePtr<FrameGraph> fg;
	std::string name;

	uint32_t refCount = 0;
};
