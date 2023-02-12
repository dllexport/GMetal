#include <PipelineBuilder.h>
#include <ReadFile.h>
#include <IntrusivePtr.h>

PipelineBuilder::PipelineBuilder(IntrusivePtr<VulkanContext>& context) : context(context)
{
	pipeline = gmetal::make_intrusive<Pipeline>();
}

PipelineBuilder& PipelineBuilder::SetRenderPass(IntrusivePtr<RenderPass>& renderPass, uint32_t subPassIndex)
{
	this->renderPass = renderPass;
	this->subPassIndex = subPassIndex;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetVertexShader(std::string path)
{
	this->vertexShaderPath = path;
	pipeline->type = Pipeline::Type::GRAPHICS;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetFragmentShader(std::string path)
{
	this->fragmentShaderPath = path;
	pipeline->type = Pipeline::Type::GRAPHICS;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetComputeShader(std::string path)
{
	this->computeShaderPath = path;
	pipeline->type = Pipeline::Type::COMPUTE;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetVertexInput(std::vector<VertexComponent> components)
{
	this->vertexComponents = components;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetVertexAssembly(VkPrimitiveTopology topology, VkBool32 restart)
{
	this->topology = topology;
	this->primitiveRestartEnable = restart;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetRasterizer(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, float lineWidth)
{
	this->polygonMode = polygonMode;
	this->cullMode = cullMode;
	this->frontFace = frontFace;
	this->lineWidth = lineWidth;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetBlendAttachmentState(std::vector<VkPipelineColorBlendAttachmentState>&& states)
{
	this->pcbas = states;
	return *this;
}

PipelineBuilder& PipelineBuilder::AddDescriptorSetLayoutBinding(std::vector<VkDescriptorSetLayoutBinding>&& bindings)
{
	this->descriptorSetLayoutBindings.push_back(bindings);
	return *this;
}

void PipelineBuilder::BuildShaderStage()
{
	auto vertShaderCode = ReadFile(vertexShaderPath.c_str());
	auto fragShaderCode = ReadFile(fragmentShaderPath.c_str());

	auto createShaderModule = [&](std::vector<char> code) {
		VkShaderModuleCreateInfo smci{};
		smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		smci.codeSize = code.size();
		smci.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(context->GetVkDevice(), &smci, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module");
		}
		return shaderModule;
	};

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	this->pipeline->shaderModules = { vertShaderModule, fragShaderModule };

	// TODO: check compute shader source
	{
		VkPipelineShaderStageCreateInfo pssci{};
		pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssci.stage = VK_SHADER_STAGE_VERTEX_BIT;
		pssci.module = vertShaderModule;
		pssci.pName = "main";
		this->shaderStageCIs.push_back(pssci);
	}

	{
		VkPipelineShaderStageCreateInfo pssci{};
		pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pssci.module = fragShaderModule;
		pssci.pName = "main";
		this->shaderStageCIs.push_back(pssci);
	}
}

void PipelineBuilder::BuildDynamicState()
{
	pdsci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pdsci.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	pdsci.pDynamicStates = dynamicStates.data();
}

void PipelineBuilder::BuildVertexInput()
{
	pvisci = Vertex::GetPipelineVertexInputState(this->vertexComponents);
}

void PipelineBuilder::BuildInputAssembly()
{
	piasci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	piasci.topology = topology;
	piasci.primitiveRestartEnable = primitiveRestartEnable;
}

void PipelineBuilder::BuildViewPort()
{
	pvsci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pvsci.viewportCount = 1;
	pvsci.scissorCount = 1;
}

void PipelineBuilder::BuildDepthStencilState()
{
	pdssci.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pdssci.depthTestEnable = VK_TRUE;
	pdssci.depthWriteEnable = VK_TRUE;
	pdssci.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	pdssci.depthBoundsTestEnable = VK_FALSE;
	pdssci.back.failOp = VK_STENCIL_OP_KEEP;
	pdssci.back.passOp = VK_STENCIL_OP_KEEP;
	pdssci.back.compareOp = VK_COMPARE_OP_ALWAYS;
	pdssci.stencilTestEnable = VK_FALSE;
	pdssci.front = pdssci.back;
}


void PipelineBuilder::BuildRasterizationState()
{
	prsci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	prsci.depthClampEnable = VK_FALSE;
	prsci.rasterizerDiscardEnable = VK_FALSE;
	prsci.polygonMode = this->polygonMode;
	prsci.lineWidth = this->lineWidth;
	prsci.cullMode = this->cullMode;
	prsci.frontFace = this->frontFace;
	prsci.depthBiasEnable = VK_FALSE;
	prsci.depthBiasConstantFactor = 0.0f; 
	prsci.depthBiasClamp = 0.0f;
	prsci.depthBiasSlopeFactor = 0.0f;
}

void PipelineBuilder::BuildMultisampleState()
{
	pmsci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pmsci.sampleShadingEnable = VK_FALSE;
	pmsci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pmsci.minSampleShading = 1.0f; // Optional
	pmsci.pSampleMask = nullptr; // Optional
	pmsci.alphaToCoverageEnable = VK_FALSE; // Optional
	pmsci.alphaToOneEnable = VK_FALSE; // Optional
}

void PipelineBuilder::BuildColorBlendAttachmentState()
{
	pcbsci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pcbsci.logicOpEnable = VK_FALSE;
	pcbsci.logicOp = VK_LOGIC_OP_COPY;
	pcbsci.attachmentCount = pcbas.size();
	pcbsci.pAttachments = pcbas.data();
	pcbsci.blendConstants[0] = 0.0f;
	pcbsci.blendConstants[1] = 0.0f;
	pcbsci.blendConstants[2] = 0.0f;
	pcbsci.blendConstants[3] = 0.0f;
}

void PipelineBuilder::BuildPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(context->GetVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout");
	}
}

void PipelineBuilder::BuildDescriptorSetLayout()
{
	descriptorSetLayouts.resize(descriptorSetLayoutBindings.size());
	for (int i = 0; i < descriptorSetLayoutBindings.size(); i++)
	{
		auto& bindings = descriptorSetLayoutBindings[i];
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pBindings = bindings.data();
		descriptorSetLayoutCreateInfo.bindingCount = bindings.size();
		vkCreateDescriptorSetLayout(context->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayouts[i]);
	}
	this->pipeline->descriptorSetLayouts = descriptorSetLayouts;
}

void PipelineBuilder::BuildDescriptorSet()
{

}

IntrusivePtr<Pipeline> PipelineBuilder::Build()
{
	BuildShaderStage();
	BuildDynamicState();
	BuildVertexInput();
	BuildInputAssembly();
	BuildViewPort();
	BuildDepthStencilState();
	BuildRasterizationState();
	BuildMultisampleState();
	BuildColorBlendAttachmentState();

	BuildDescriptorSetLayout();
	BuildPipelineLayout();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = this->shaderStageCIs.size();
	pipelineInfo.pStages = this->shaderStageCIs.data();
	pipelineInfo.pVertexInputState = &pvisci;
	pipelineInfo.pInputAssemblyState = &piasci;
	pipelineInfo.pViewportState = &pvsci;
	pipelineInfo.pRasterizationState = &prsci;
	pipelineInfo.pMultisampleState = &pmsci;
	pipelineInfo.pDepthStencilState = &pdssci;
	pipelineInfo.pColorBlendState = &pcbsci;
	pipelineInfo.pDynamicState = &pdsci;

	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass->renderPass;
	pipelineInfo.subpass = subPassIndex;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	VkPipeline graphicsPipeline;
	if (vkCreateGraphicsPipelines(context->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipeline");
	}

	pipeline->context = context;
	pipeline->pipelineLayout = pipelineLayout;
	pipeline->pipeline = graphicsPipeline;
	return pipeline;
}

VkDescriptorSetLayoutBinding PipelineBuilder::BuildDescriptorSetLayoutBinding(VkDescriptorType type, VkPipelineStageFlags stageFlags, uint32_t binding, VkSampler* samplers, uint32_t count)
{
	VkDescriptorSetLayoutBinding setLayoutBinding = {};
	setLayoutBinding.descriptorType = type;
	setLayoutBinding.stageFlags = stageFlags;
	setLayoutBinding.binding = binding;
	setLayoutBinding.descriptorCount = count;
	setLayoutBinding.pImmutableSamplers = samplers;
	return setLayoutBinding;
}
