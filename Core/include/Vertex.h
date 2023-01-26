#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

enum class VertexComponent { Position, Normal, UV, Color, Tangent, Joint0, Weight0 };

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 color;
	glm::vec4 joint0;
	glm::vec4 weight0;
	glm::vec4 tangent;
	
	/** @brief Returns the default pipeline vertex input state create info structure for the requested vertex components */
	static VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputState(const std::vector<VertexComponent> components);
};