#pragma once

#include <ImageResourceNode.h>
#include <ResourceNodeVisitor.h>

class DepthStencilImageResourceNode : public ImageResourceNode {
public:
    DepthStencilImageResourceNode(VkFormat format) : ImageResourceNode(format) {
        this->SetDepthStencil();
    }

    DepthStencilImageResourceNode(IntrusivePtr<Image>& image) : ImageResourceNode(image) {
        this->SetDepthStencil();
    }

    virtual void Accept(ResourceNodeVisitor* visitor) { visitor->Visit(this); }
};