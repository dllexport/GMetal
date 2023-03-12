#pragma once

#include <vulkan/vulkan.h>
#include <Pipeline.h>

class Material : public IntrusiveCounter<Material>
{
public:
	virtual void Bind(VkCommandBuffer& commandBuffer) = 0;
};