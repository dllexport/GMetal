#include <UniformResourceNode.h>

UniformResourceNode::UniformResourceNode() {

}

UniformResourceNode::~UniformResourceNode() {

}

void UniformResourceNode::Accept(ResourceNodeVisitor* visitor) { 
	visitor->Visit(this); 
}

void UniformResourceNode::Resolve(VkExtent3D extend) {

}