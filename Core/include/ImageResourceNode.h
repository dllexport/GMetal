#pragma once

#include <FrameGraph.h>
#include <FrameGraphNode.h>
#include <Image.h>

class ImageResourceNode : public FrameGraphNode
{
public:
	ImageResourceNode() {}
	virtual ~ImageResourceNode() {}

	void Assign(IntrusivePtr<Image>& image) {}

private:
	IntrusivePtr<Image>& image;
};
