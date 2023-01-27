#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <VulkanContext.h>
#include <Pipeline.h>
#include <Vertex.h>
#include <RenderPass.h>

class PipelineBuilder
{
public:
	PipelineBuilder(IntrusivePtr<VulkanContext>& context);
	PipelineBuilder& SetRenderPass(IntrusivePtr<RenderPass>& renderPass, uint32_t subPassIndex);
	PipelineBuilder& SetVertexShader(std::string path);
	PipelineBuilder& SetFragmentShader(std::string path);
	PipelineBuilder& SetComputeShader(std::string path);
	PipelineBuilder& SetVertexInput(std::vector<VertexComponent> components);
	PipelineBuilder& SetVertexAssembly(VkPrimitiveTopology topology, VkBool32 restart = VK_FALSE);
	PipelineBuilder& SetRasterizer(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, float lineWidth = 1.0f);

	PipelineBuilder& AddDescriptorSetLayoutBinding(std::vector<VkDescriptorSetLayoutBinding>&& bindings);

	IntrusivePtr<Pipeline> Build();

	static VkDescriptorSetLayoutBinding BuildDescriptorSetLayoutBinding(VkDescriptorType type, VkPipelineStageFlags stageFlags, uint32_t binding, VkSampler* samplers = nullptr, uint32_t count = 1);

private:
	IntrusivePtr<VulkanContext> context;
	IntrusivePtr<Pipeline> pipeline;
	IntrusivePtr<RenderPass> renderPass;
	uint32_t subPassIndex;

	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::string computeShaderPath;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCIs;
	void BuildShaderStage();

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo pdsci = {};
	void BuildDynamicState();

	VkPipelineVertexInputStateCreateInfo pvisci = {};
	std::vector<VertexComponent> vertexComponents;
	void BuildVertexInput();

	VkPipelineInputAssemblyStateCreateInfo piasci = {};
	VkPrimitiveTopology topology;
	VkBool32 primitiveRestartEnable;
	void BuildInputAssembly();

	VkPipelineViewportStateCreateInfo pvsci = {};
	void BuildViewPort();

	VkPipelineDepthStencilStateCreateInfo pdssci = {};
	void BuildDepthStencilState();

	VkPipelineRasterizationStateCreateInfo prsci = {};
	VkPolygonMode polygonMode;
	VkCullModeFlags cullMode;
	VkFrontFace frontFace;
	float lineWidth;
	void BuildRasterizationState();

	VkPipelineMultisampleStateCreateInfo pmsci = {};
	void BuildMultisampleState();

	VkPipelineColorBlendAttachmentState pcbas = {};
	VkPipelineColorBlendStateCreateInfo pcbsci = {};
	void BuildColorBlendAttachmentState();

	VkPipelineLayout pipelineLayout;
	void BuildPipelineLayout();

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
	void BuildDescriptorSetLayout();

	VkDescriptorSet descriptorSet;
	void BuildDescriptorSet();
};