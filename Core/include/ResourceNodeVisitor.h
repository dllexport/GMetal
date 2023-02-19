#pragma once

class ImageResourceNode;
class UniformResourceNode;
class ResourceNodeVisitor 
{
public:
    virtual void Visit(ImageResourceNode* node) = 0;
    virtual void Visit(UniformResourceNode* node) = 0;
};