#pragma once

#include <IntrusivePtr.h>
#include <vulkan/vulkan.h>
#include <Vertex.h>

class Drawable : public IntrusiveCounter<Drawable>
{
public:
	enum DrawTopology
	{
        POINT_LIST = 0,
        LINE_LIST = 1,
        LINE_STRIP = 2,
        TRIANGLE_LIST = 3,
        TRIANGLE_STRIP = 4,
        TRIANGLE_FAN = 5,
        LINE_LIST_WITH_ADJACENCY = 6,
        LINE_STRIP_WITH_ADJACENCY = 7,
        TRIANGLE_LIST_WITH_ADJACENCY = 8,
        TRIANGLE_STRIP_WITH_ADJACENCY = 9,
        PATCH_LIST = 10
	};

    enum DrawIndexType
    {
        UINT16 = 0,
        UINT32 = 1,
        NONE_KHR = 1000165000,
        UINT8_EXT = 1000265000,
        NONE_NV = NONE_KHR
    };

	void SetVertexArray(std::vector<Vertex>&& vertices);
	void SetVertexTopology(DrawTopology topology, DrawIndexType indexType);

    void Draw(VkCommandBuffer& commandBuffer);

private:
    friend class RenderPass;

    std::vector<Vertex> vertices;
    DrawTopology topology;
    DrawIndexType indexType;

    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
};