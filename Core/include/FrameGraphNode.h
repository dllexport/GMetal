#pragma once

#include <string>
#include <IntrusivePtr.h>

class FrameGraph;
class FrameGraphNode : public IntrusiveCounter<FrameGraphNode>
{
public:
	FrameGraphNode(FrameGraph* fg = nullptr);
	virtual ~FrameGraphNode() {};

	// retain the node so it won't be culled
	void Retain() { refCount++; }
	bool Valid() { return refCount > 0; }

	template <class T> T* As() { return static_cast<T*>(this); }

protected:
	friend class FrameGraph;
	FrameGraph* fg;
	std::string name;

	uint32_t refCount = 0;
};
