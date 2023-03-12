#include <Drawable.h>

void Drawable::SetVertexArray(std::vector<Vertex>&& vertices)
{
	this->vertices = vertices;
}

void Drawable::SetVertexTopology(DrawTopology topology, DrawIndexType indexType)
{
	this->topology = topology;
	this->indexType = indexType;
}

void Drawable::Draw(VkCommandBuffer& commandBuffer)
{
	const VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &this->vertexBuffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, this->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}