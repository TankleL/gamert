#include "rendermgr.hpp"
#include "resourcesmgr.hpp"
#include "vkutils.hpp"

using namespace std;

/* ==================================================================
|                         Region: Pipeline                          |
================================================================== */
RenderMgr::Pipeline::Pipeline() noexcept
	: id(0)
	, _pipeline(VK_NULL_HANDLE)
	, _pipeline_layout(VK_NULL_HANDLE)
{}

RenderMgr::Pipeline::Pipeline(
	uint32_t pipeline_id,
	const std::string& vert_shader_path,
	const std::string& frag_shader_path) noexcept
	: id(pipeline_id)
	, shader_vert_path(vert_shader_path)
	, shader_frag_path(frag_shader_path)
	, _pipeline(VK_NULL_HANDLE)
	, _pipeline_layout(VK_NULL_HANDLE)
{}

RenderMgr::Pipeline::Pipeline(const Pipeline& rhs) noexcept
	: id(rhs.id)
	, shader_vert_path(rhs.shader_vert_path)
	, shader_frag_path(rhs.shader_frag_path)
	, _pipeline(rhs._pipeline)
	, _pipeline_layout(rhs._pipeline_layout)
{}

RenderMgr::Pipeline::Pipeline(Pipeline&& rhs) noexcept
	: id(rhs.id)
	, shader_vert_path(std::move(rhs.shader_vert_path))
	, shader_frag_path(std::move(rhs.shader_frag_path))
	, _pipeline(rhs._pipeline)
	, _pipeline_layout(rhs._pipeline_layout)
{
	rhs._pipeline = VK_NULL_HANDLE;
	rhs._pipeline_layout = VK_NULL_HANDLE;
}

RenderMgr::Pipeline& RenderMgr::Pipeline::operator=(const Pipeline& rhs) noexcept
{
	id = rhs.id;
	shader_vert_path = rhs.shader_vert_path;
	shader_frag_path = rhs.shader_frag_path;
	_pipeline = rhs._pipeline;
	_pipeline_layout = rhs._pipeline_layout;
	return *this;
}

RenderMgr::Pipeline& RenderMgr::Pipeline::operator=(Pipeline&& rhs) noexcept
{
	id = rhs.id;
	shader_vert_path = std::move(rhs.shader_vert_path);
	shader_frag_path = std::move(rhs.shader_frag_path);
	_pipeline = rhs._pipeline;					rhs._pipeline = VK_NULL_HANDLE;
	_pipeline_layout = rhs._pipeline_layout;	rhs._pipeline_layout = VK_NULL_HANDLE;
	return *this;
}


/* ==================================================================
|                      Region: Render Manager                       |
================================================================== */

VNode RenderMgr::_dummy_vtree;

RenderMgr::RenderMgr()
	: _vtree(&_dummy_vtree)
	, _vkdevice(VK_NULL_HANDLE)
	, _vkphydev(VK_NULL_HANDLE)
	, _vkrdrpass(VK_NULL_HANDLE)
	, _vkscext{ 0 }
{
	_pipelines[BTPL_2D_Position_RGBColor] = Pipeline(
		BTPL_2D_Position_RGBColor,
		"shaders/demo.vert.spv",
		"shaders/demo.frag.spv");
}

RenderMgr& RenderMgr::get_instance()
{
	static RenderMgr rmgr;
	return rmgr;
}

void RenderMgr::render_frame()
{
	_vtree->render();
}

void RenderMgr::set_vulkan_device(VkDevice device)
{
	_vkdevice = device;
}

void RenderMgr::set_vulkan_physical_device(VkPhysicalDevice phydev)
{
	_vkphydev = phydev;
}

void RenderMgr::set_vulkan_renderpass(VkRenderPass renderpass)
{
	_vkrdrpass = renderpass;
}

void RenderMgr::set_vulkan_swapchain_extent(VkExtent2D extent)
{
	_vkscext = extent;
}

VkDevice RenderMgr::get_vulkan_device() const
{
	return _vkdevice;
}

VkPhysicalDevice RenderMgr::get_vulkan_physical_device() const
{
	return _vkphydev;
}

void RenderMgr::create_pipelines()
{
	_create_builtin_pipelines();
}

void RenderMgr::destroy_pipelines()
{
	for (auto& ppl : _pipelines)
	{
		vkDestroyPipeline(_vkdevice, ppl.second._pipeline, nullptr);
		vkDestroyPipelineLayout(_vkdevice, ppl.second._pipeline_layout, nullptr);

		ppl.second._pipeline = VK_NULL_HANDLE;
		ppl.second._pipeline_layout = VK_NULL_HANDLE;
	}
}

VkPipeline RenderMgr::get_vulkan_pipeline(BuiltinPipelines id)
{
	auto ppl = _pipelines.find(id);
	assert(ppl != _pipelines.end());
	return ppl->second._pipeline;
}

void RenderMgr::_create_builtin_pipelines()
{
	ResourcesMgr& resmgr = ResourcesMgr::get_instance();
	std::vector<std::uint8_t> shader_code;

	/* -----------------------------------------------------------------
	|                     BTPL_2D_Position_RGBColor                    |
	----------------------------------------------------------------- */
	auto& ppl = _pipelines[BTPL_2D_Position_RGBColor];

	resmgr.read_binary_file(shader_code, ppl.shader_vert_path);
	VkPipelineShaderStageCreateInfo vs_stage_info = {};
	vs_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vs_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vs_stage_info.module = VkUtils::create_shader_module(_vkdevice, shader_code);
	vs_stage_info.pName = "main";

	resmgr.read_binary_file(shader_code, ppl.shader_frag_path);
	VkPipelineShaderStageCreateInfo fs_stage_info = {};
	fs_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fs_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fs_stage_info.module = VkUtils::create_shader_module(_vkdevice, shader_code);
	fs_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo shader_stage_infos[] = { vs_stage_info, fs_stage_info };

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 0;
	vertex_input_info.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)_vkscext.width;
	viewport.height = (float)_vkscext.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = _vkscext;

	VkPipelineViewportStateCreateInfo viewport_state_info = {};
	viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_info.viewportCount = 1;
	viewport_state_info.pViewports = &viewport;
	viewport_state_info.scissorCount = 1;
	viewport_state_info.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer_info = {};
	rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = VK_FALSE;
	rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
	rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer_info.lineWidth = 1.0f;
	rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer_info.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling_info = {};
	multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.sampleShadingEnable = VK_FALSE;
	multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo color_blending_info = {};
	color_blending_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending_info.logicOpEnable = VK_FALSE;
	color_blending_info.logicOp = VK_LOGIC_OP_COPY;
	color_blending_info.attachmentCount = 1;
	color_blending_info.pAttachments = &color_blend_attachment;
	color_blending_info.blendConstants[0] = 0.0f;
	color_blending_info.blendConstants[1] = 0.0f;
	color_blending_info.blendConstants[2] = 0.0f;
	color_blending_info.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 0;
	pipeline_layout_info.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(_vkdevice, &pipeline_layout_info, nullptr, &ppl._pipeline_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stage_infos;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly_info;
	pipeline_info.pViewportState = &viewport_state_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pColorBlendState = &color_blending_info;
	pipeline_info.layout = ppl._pipeline_layout;
	pipeline_info.renderPass = _vkrdrpass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(_vkdevice, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &ppl._pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(_vkdevice, vs_stage_info.module, nullptr);
	vkDestroyShaderModule(_vkdevice, fs_stage_info.module, nullptr);
}
