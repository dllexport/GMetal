#pragma once

#include <string>
#include <IntrusivePtr.h>

class FrameGraph;
class FrameGraphNode : public IntrusiveCounter<FrameGraphNode>
{
public:
	FrameGraphNode(IntrusivePtr<FrameGraph> fg = nullptr);
	virtual ~FrameGraphNode() {};

protected:
	friend class FrameGraph;
	IntrusivePtr<FrameGraph> fg;
	std::string name;
};
