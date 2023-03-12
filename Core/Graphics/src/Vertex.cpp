#include <Vertex.h>

static VkVertexInputAttributeDescription InputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component) {
	switch (component) {
	case VertexComponent::Position:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) });
	case VertexComponent::Normal:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	case VertexComponent::UV:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
	case VertexComponent::Color:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color) });
	case VertexComponent::Tangent:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, tangent) });
	case VertexComponent::Joint0:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, joint0) });
	case VertexComponent::Weight0:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, weight0) });
	default:
		return VkVertexInputAttributeDescription({});
	}
}

VkPipelineVertexInputStateCreateInfo Vertex::GetPipelineVertexInputState(const std::vector<VertexComponent> components)
{
	static VkVertexInputBindingDescription vertexInputBindingDescription;
	static std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;

	vertexInputBindingDescription = { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX };
	vertexAttributeDescriptions.clear();

	uint32_t location = 0;
	for (VertexComponent component : components) {
		vertexAttributeDescriptions.push_back(InputAttributeDescription(0, location, component));
		location++;
	}

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vertexAttributeDescriptions.empty() ? 0 : 1;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
	return pipelineVertexInputStateCreateInfo;
}
