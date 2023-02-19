#pragma once

#include <FrameGraph.h>
#include <FrameGraphNode.h>
#include <ResourceNodeVisitor.h>

class ResourceNode : public FrameGraphNode
{
public:
	ResourceNode() {}
	virtual ~ResourceNode() {}
	virtual void Accept(ResourceNodeVisitor* visitor) = 0;
};
