#include <UniformResourceNode.h>

#include <vma/vk_mem_alloc.h>

#include <GeneralBufferBuilder.h>

UniformResourceNode::UniformResourceNode() {

}

UniformResourceNode::~UniformResourceNode() {

}

void UniformResourceNode::Accept(ResourceNodeVisitor* visitor) { 
	visitor->Visit(this); 
}

void UniformResourceNode::Resolve(VkExtent3D extend) { 
	this->gpuBuffer = GeneralBufferBuilder(this->fg->Context())
            .SetBasic(this->length, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
            .Build();
}