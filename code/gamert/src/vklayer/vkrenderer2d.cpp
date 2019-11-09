#include "vkrenderer2d.hpp"
#include "vkcontext.hpp"
#include "vkutils.hpp"
#include "vvertex.hpp"
#include "resourcesmgr.hpp"

using namespace std;

VSceneGraph VKRenderer2d::_dummy_graph;

VKRenderer2d::VKRenderer2d()
	: _render_pass(VK_NULL_HANDLE)
	, _graphics_pipeline(VK_NULL_HANDLE)
	, _graphics_pipeline_layout(VK_NULL_HANDLE)
	, _desc_pool(VK_NULL_HANDLE)
	, _desc_static_sl(VK_NULL_HANDLE)
	, _desc_dynamic_sl(VK_NULL_HANDLE)
	, _scene_graph(&_dummy_graph)
	, _swapchain(nullptr)
	, _camera(nullptr)
	, _initialized(false)
{}

void VKRenderer2d::set_scene_graph(VSceneGraph* graph)
{
	if (graph)
		_scene_graph = graph;
	else
		_scene_graph = &_dummy_graph;
}

void VKRenderer2d::set_camera(VCamera* camera)
{
	_camera = camera;
}

void VKRenderer2d::init(VKSwapchain* swapchain)
{
	if (!_initialized)
	{
		_swapchain = swapchain;
		_create_render_pass();
		_create_framebuffers();
		_create_descriptor_set_layout();
		_create_graphics_pipeline();
		_create_uniform_buffers();
		_create_descriptor_pool();
		_create_descriptor_set();
		_create_primary_commandbuffers();
		_ensure_stable_uniform_data();
		_initialized = true;
	}
}

void VKRenderer2d::unint()
{
	if (_initialized)
	{
		_destroy_primary_commandbuffers();
		_destroy_descriptor_set();
		_destroy_descriptor_pool();
		_destroy_uniform_buffers();
		_destroy_graphics_pipeline();
		_destroy_descriptor_set_layout();
		_destroy_framebuffers();
		_destroy_render_pass();
		_initialized = false;
	}
}

void VKRenderer2d::_create_render_pass()
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = _swapchain->get_vulkan_image_format();
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	GRT_CHECK(
		VK_SUCCESS == vkCreateRenderPass(
			VKContext::get_instance().get_vulkan_device(),
			&render_pass_info,
			nullptr,
			&_render_pass),
		"failed to create render pass!");
}

void VKRenderer2d::_destroy_render_pass()
{
	vkDestroyRenderPass(
		VKContext::get_instance().get_vulkan_device(),
		_render_pass,
		nullptr);
	_render_pass = VK_NULL_HANDLE;
}

void VKRenderer2d::_create_framebuffers()
{
	GRT_CHECK(
		VK_NULL_HANDLE != _render_pass,
		"empty render pass.");

	const std::vector<VkImageView> img_views = _swapchain->get_vulkan_image_views();
	_frame_buffers.resize(img_views.size());
	const VkExtent2D& extent = _swapchain->get_vulkan_extent();

	for (size_t i = 0; i < img_views.size(); ++i)
	{
		VkImageView attachments[] =
		{
			img_views.at(i)
		};

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = _render_pass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = extent.width;
		framebuffer_info.height = extent.height;
		framebuffer_info.layers = 1;

		GRT_CHECK(
			VK_SUCCESS == vkCreateFramebuffer(
				VKContext::get_instance().get_vulkan_device(),
				&framebuffer_info,
				nullptr,
				&_frame_buffers[i]),
			"failed to create framebuffer.");
	}
}

void VKRenderer2d::_destroy_framebuffers()
{
	for (auto fb : _frame_buffers)
	{
		vkDestroyFramebuffer(
			VKContext::get_instance().get_vulkan_device(),
			fb,
			nullptr);
	}
	_frame_buffers.clear();
}

void VKRenderer2d::_create_graphics_pipeline()
{
	ResourcesMgr& resmgr = ResourcesMgr::get_instance();
	const VkDevice vkdev = VKContext::get_instance().get_vulkan_device();
	std::vector<std::uint8_t> vs_code;
	std::vector<std::uint8_t> fs_code;
	
	resmgr.read_binary_file(vs_code, "shaders/simple_2drgb.vert.spv");
	resmgr.read_binary_file(fs_code, "shaders/simple_2drgb.frag.spv");


	VkShaderModule vs_module = VKUtils::create_shader_module(vkdev, vs_code);
	VkShaderModule fs_module = VKUtils::create_shader_module(vkdev, fs_code);
	
	VkPipelineShaderStageCreateInfo vs_stage_info = {};
	vs_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vs_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vs_stage_info.module = vs_module;
	vs_stage_info.pName = "main";

	
	VkPipelineShaderStageCreateInfo fs_stage_info = {};
	fs_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fs_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fs_stage_info.module = fs_module;
	fs_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo shader_stage_infos[] =
		{ vs_stage_info, fs_stage_info };

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 1;
	vertex_input_info.pVertexBindingDescriptions = 
		&(VVertex2DRGBDescriptor::get_instance().binding_description());
	vertex_input_info.vertexAttributeDescriptionCount =
		(uint32_t)VVertex2DRGBDescriptor::get_instance().attribute_description().size();
	vertex_input_info.pVertexAttributeDescriptions =
		VVertex2DRGBDescriptor::get_instance().attribute_description().data();
	
	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = VK_FALSE;

	const VkExtent2D& extent = _swapchain->get_vulkan_extent();
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;

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
	color_blend_attachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
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

	VkDescriptorSetLayout dslayouts[] = { _desc_static_sl, _desc_dynamic_sl };
	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 2;
	pipeline_layout_info.pSetLayouts = dslayouts;

	GRT_CHECK(
		VK_SUCCESS == vkCreatePipelineLayout(
			vkdev,
			&pipeline_layout_info,
			nullptr,
			&_graphics_pipeline_layout),
		"failed to create pipeline layout.");

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
	pipeline_info.layout = _graphics_pipeline_layout;
	pipeline_info.renderPass = _render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	GRT_CHECK(
		VK_SUCCESS == vkCreateGraphicsPipelines(
			vkdev,
			VK_NULL_HANDLE,
			1,
			&pipeline_info,
			nullptr,
			&_graphics_pipeline),
		"failed to create graphics pipeline.");

	vkDestroyShaderModule(vkdev, vs_module, nullptr);
	vkDestroyShaderModule(vkdev, fs_module, nullptr);
}

void VKRenderer2d::_destroy_graphics_pipeline()
{
	const VkDevice vkdev = VKContext::get_instance().get_vulkan_device();
	vkDestroyPipeline(vkdev, _graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(vkdev, _graphics_pipeline_layout, nullptr);

	_graphics_pipeline = VK_NULL_HANDLE;
	_graphics_pipeline_layout = VK_NULL_HANDLE;
}

void VKRenderer2d::_create_primary_commandbuffers()
{
	int buffer_count = (int)_swapchain->get_vulkan_image_views().size();

	_primary_cmds.resize(buffer_count);

	VkCommandBufferAllocateInfo cmd_info = {};
	cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd_info.commandBufferCount = buffer_count;
	cmd_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd_info.commandPool = VKContext::get_instance().get_vulkan_command_pool();

	GRT_CHECK(
		VK_SUCCESS == vkAllocateCommandBuffers(
			VKContext::get_instance().get_vulkan_device(),
			&cmd_info,
			_primary_cmds.data()),
		"failed to allocate primary command buffers.");
}

void VKRenderer2d::_destroy_primary_commandbuffers()
{
	vkFreeCommandBuffers(
		VKContext::get_instance().get_vulkan_device(),
		VKContext::get_instance().get_vulkan_command_pool(),
		(uint32_t)_primary_cmds.size(),
		_primary_cmds.data());
	_primary_cmds.clear();
}

void VKRenderer2d::_create_uniform_buffers()
{
	const VkDevice			vkdev		= VKContext::get_instance().get_vulkan_device();
	const VkPhysicalDevice	vkphydev	= VKContext::get_instance().get_vulkan_physical_device();
	int						img_count	= (int)_swapchain->get_vulkan_image_views().size();

	const int MAX_DRAW_CALLS = 1024;
	
	const VkDeviceSize size_ubuf_stable = sizeof(_ubuf_stable_t);
	const VkDeviceSize size_ubuf_single_dc = sizeof(_ubuf_combined_dc_t);
	const VkDeviceSize size_ubuf_combined_dc = sizeof(_ubuf_combined_dc_t);

	_ubuf_stable.resize(img_count);
	_ubuf_stable_mem.resize(img_count);

	_ubuf_single_dc.resize(img_count);
	_ubuf_single_dc_mem.resize(img_count);

	_ubuf_combined_dc.resize(img_count);
	_ubuf_combined_dc_mem.resize(img_count);

	for (int i = 0; i < img_count; ++i)
	{
		VKUtils::create_buffer(
			_ubuf_stable[i],
			_ubuf_stable_mem[i],
			vkdev,
			vkphydev,
			size_ubuf_stable,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VKUtils::create_buffer(
			_ubuf_combined_dc[i],
			_ubuf_combined_dc_mem[i],
			vkdev,
			vkphydev,
			size_ubuf_combined_dc,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VKUtils::create_buffer(
			_ubuf_single_dc[i],
			_ubuf_single_dc_mem[i],
			vkdev,
			vkphydev,
			size_ubuf_single_dc * MAX_DRAW_CALLS,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void VKRenderer2d::_destroy_uniform_buffers()
{
	const VkDevice	vkdev = VKContext::get_instance().get_vulkan_device();

	{
		const auto cnt = _ubuf_stable.size();
		for (size_t i = 0; i < cnt; ++i)
		{
			vkDestroyBuffer(vkdev, _ubuf_stable[i], nullptr);
			vkFreeMemory(vkdev, _ubuf_stable_mem[i], nullptr);
		}
		_ubuf_stable.clear();
		_ubuf_stable_mem.clear();
	}

	{
		const auto cnt = _ubuf_combined_dc.size();
		for (size_t i = 0; i < cnt; ++i)
		{
			vkDestroyBuffer(vkdev, _ubuf_combined_dc[i], nullptr);
			vkFreeMemory(vkdev, _ubuf_combined_dc_mem[i], nullptr);
		}
		_ubuf_combined_dc.clear();
		_ubuf_combined_dc_mem.clear();
	}

	{
		const auto cnt = _ubuf_single_dc.size();
		for (size_t i = 0; i < cnt; ++i)
		{
			vkDestroyBuffer(vkdev, _ubuf_single_dc[i], nullptr);
			vkFreeMemory(vkdev, _ubuf_single_dc_mem[i], nullptr);
		}
		_ubuf_single_dc.clear();
		_ubuf_single_dc_mem.clear();
	}
}

void VKRenderer2d::_create_descriptor_pool()
{
	const uint32_t img_count = static_cast<uint32_t>(
		_swapchain->get_vulkan_image_views().size());
	VkDescriptorPoolSize psi[UBT_Counts];
	memset(psi, 0, sizeof(psi));

	psi[UBT_StableInfo].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	psi[UBT_StableInfo].descriptorCount = img_count;

	psi[UBT_CombinedDC].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	psi[UBT_CombinedDC].descriptorCount = img_count;

	psi[UBT_SingleDC].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	psi[UBT_SingleDC].descriptorCount = img_count;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = UBT_Counts;
	pool_info.pPoolSizes = psi;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = img_count * 2;		// for static and dynamic sets

	GRT_CHECK(
		VK_SUCCESS == vkCreateDescriptorPool(
			VKContext::get_instance().get_vulkan_device(),
			&pool_info,
			nullptr,
			&_desc_pool),
		"failed to create descriptor pool.");
}

void VKRenderer2d::_destroy_descriptor_pool()
{
	if (VK_NULL_HANDLE != _desc_pool)
	{
		vkDestroyDescriptorPool(
			VKContext::get_instance().get_vulkan_device(),
			_desc_pool,
			nullptr);
	}
}

void VKRenderer2d::_create_descriptor_set_layout()
{
	VkDescriptorSetLayoutBinding dlbs[UBT_Counts];	// descriptor-set layout binding
	memset(dlbs, 0, sizeof(dlbs));

	dlbs[UBT_StableInfo].binding = UBT_StableInfo;		// 0
	dlbs[UBT_StableInfo].descriptorCount = 1;
	dlbs[UBT_StableInfo].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	dlbs[UBT_StableInfo].pImmutableSamplers = nullptr;
	dlbs[UBT_StableInfo].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	dlbs[UBT_CombinedDC].binding = UBT_CombinedDC;		// 1
	dlbs[UBT_CombinedDC].descriptorCount = 1;
	dlbs[UBT_CombinedDC].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	dlbs[UBT_CombinedDC].pImmutableSamplers = nullptr;
	dlbs[UBT_CombinedDC].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	dlbs[UBT_SingleDC].binding = UBT_SingleDC;			// 2
	dlbs[UBT_SingleDC].descriptorCount = 1;
	dlbs[UBT_SingleDC].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	dlbs[UBT_SingleDC].pImmutableSamplers = nullptr;
	dlbs[UBT_SingleDC].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layouts_info = {};
	layouts_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layouts_info.bindingCount = UBT_Counts - 1;
	layouts_info.pBindings = dlbs;

	GRT_CHECK(
		VK_SUCCESS == vkCreateDescriptorSetLayout(
			VKContext::get_instance().get_vulkan_device(),
			&layouts_info,
			nullptr,
			&_desc_static_sl),
		"failed to create descriptor set layout.");

	layouts_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layouts_info.bindingCount = 1;
	layouts_info.pBindings = &dlbs[UBT_SingleDC];


	GRT_CHECK(
		VK_SUCCESS == vkCreateDescriptorSetLayout(
			VKContext::get_instance().get_vulkan_device(),
			&layouts_info,
			nullptr,
			&_desc_dynamic_sl),
		"failed to create descriptor set layout.");
}

void VKRenderer2d::_destroy_descriptor_set_layout()
{
	if (VK_NULL_HANDLE != _desc_static_sl)
	{
		vkDestroyDescriptorSetLayout(
			VKContext::get_instance().get_vulkan_device(),
			_desc_static_sl,
			nullptr);
		_desc_static_sl = VK_NULL_HANDLE;
	}

	if (VK_NULL_HANDLE != _desc_dynamic_sl)
	{
		vkDestroyDescriptorSetLayout(
			VKContext::get_instance().get_vulkan_device(),
			_desc_dynamic_sl,
			nullptr);
		_desc_dynamic_sl = VK_NULL_HANDLE;
	}
}

void VKRenderer2d::_create_descriptor_set()
{
	const size_t img_count = _swapchain->get_vulkan_image_views().size();

	std::vector<VkDescriptorSetLayout> layouts(
		img_count,
		_desc_static_sl);
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = _desc_pool;
	alloc_info.descriptorSetCount = static_cast<uint32_t>(img_count);
	alloc_info.pSetLayouts = layouts.data();

	_desc_static_sets.resize(img_count);
	GRT_CHECK(
		VK_SUCCESS == vkAllocateDescriptorSets(
			VKContext::get_instance().get_vulkan_device(),
			&alloc_info,
			_desc_static_sets.data()),
		"failed to allocate descriptor sets.");

	layouts.assign(img_count, _desc_dynamic_sl);
	alloc_info.pSetLayouts = layouts.data();
	_desc_dynamic_sets.resize(img_count);
	GRT_CHECK(
		VK_SUCCESS == vkAllocateDescriptorSets(
			VKContext::get_instance().get_vulkan_device(),
			&alloc_info,
			_desc_dynamic_sets.data()),
		"failed to allocate descriptor sets.");

	for (size_t i = 0; i < img_count; ++i)
	{
		// uniform buffers information
		VkDescriptorBufferInfo buffer_infos[UBT_Counts];
		memset(buffer_infos, 0, sizeof(buffer_infos));

		buffer_infos[UBT_StableInfo].buffer = _ubuf_stable[i];
		buffer_infos[UBT_StableInfo].offset = 0;
		buffer_infos[UBT_StableInfo].range = sizeof(_ubuf_stable_t);

		buffer_infos[UBT_CombinedDC].buffer = _ubuf_combined_dc[i];
		buffer_infos[UBT_CombinedDC].offset = 0;
		buffer_infos[UBT_CombinedDC].range = sizeof(_ubuf_combined_dc_t);

		buffer_infos[UBT_SingleDC].buffer = _ubuf_single_dc[i];
		buffer_infos[UBT_SingleDC].offset = 0;
		buffer_infos[UBT_SingleDC].range = sizeof(_ubuf_single_dc_t);

		// writers
		VkWriteDescriptorSet desc_writes[UBT_Counts];
		memset(desc_writes, 0, sizeof(desc_writes));

		desc_writes[UBT_StableInfo].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		desc_writes[UBT_StableInfo].dstSet = _desc_static_sets[i];
		desc_writes[UBT_StableInfo].dstBinding = UBT_StableInfo;
		desc_writes[UBT_StableInfo].dstArrayElement = 0;
		desc_writes[UBT_StableInfo].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		desc_writes[UBT_StableInfo].descriptorCount = 1;
		desc_writes[UBT_StableInfo].pBufferInfo = &buffer_infos[UBT_StableInfo];

		desc_writes[UBT_CombinedDC].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		desc_writes[UBT_CombinedDC].dstSet = _desc_static_sets[i];
		desc_writes[UBT_CombinedDC].dstBinding = 0;
		desc_writes[UBT_CombinedDC].dstArrayElement = UBT_CombinedDC;
		desc_writes[UBT_CombinedDC].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		desc_writes[UBT_CombinedDC].descriptorCount = 1;
		desc_writes[UBT_CombinedDC].pBufferInfo = &buffer_infos[UBT_CombinedDC];

		desc_writes[UBT_SingleDC].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		desc_writes[UBT_SingleDC].dstSet = _desc_dynamic_sets[i];
		desc_writes[UBT_SingleDC].dstBinding = UBT_SingleDC;
		desc_writes[UBT_SingleDC].dstArrayElement = 0;
		desc_writes[UBT_SingleDC].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		desc_writes[UBT_SingleDC].descriptorCount = 1;
		desc_writes[UBT_SingleDC].pBufferInfo = &buffer_infos[UBT_SingleDC];

		vkUpdateDescriptorSets(
			VKContext::get_instance().get_vulkan_device(),
			UBT_Counts,
			desc_writes,
			0,
			nullptr);
	}
}

void VKRenderer2d::_destroy_descriptor_set()
{
	if(_desc_static_sets.size() > 0)
	{
		vkFreeDescriptorSets(
			VKContext::get_instance().get_vulkan_device(),
			_desc_pool,
			(uint32_t)_desc_static_sets.size(),
			_desc_static_sets.data());
	}
	_desc_static_sets.clear();

	if (_desc_dynamic_sets.size() > 0)
	{
		vkFreeDescriptorSets(
			VKContext::get_instance().get_vulkan_device(),
			_desc_pool,
			(uint32_t)_desc_dynamic_sets.size(),
			_desc_dynamic_sets.data());
	}
	_desc_dynamic_sets.clear();
}

void VKRenderer2d::_ensure_stable_uniform_data()
{
	const VkDevice& vkdev =
		VKContext::get_instance().get_vulkan_device();
	const VkPhysicalDevice& vkphydev = VKContext::get_instance().get_vulkan_physical_device();
	const VkExtent2D& vkext = 
		_swapchain->get_vulkan_extent();

	for (const auto& ubuf : _ubuf_stable)
	{
		_ubuf_stable_t ubo;
		ubo.extent[0] = (float)vkext.width;
		ubo.extent[1] = (float)vkext.height;

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_mem;
		VKUtils::create_buffer(
			staging_buffer,
			staging_buffer_mem,
			vkdev,
			vkphydev,
			sizeof(ubo),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data = nullptr;
		vkMapMemory(
			vkdev,
			staging_buffer_mem,
			0,
			sizeof(ubo),
			0,
			&data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(vkdev, staging_buffer_mem);

		VKUtils::copy_buffer(
			ubuf,
			staging_buffer,
			sizeof(ubo),
			vkdev,
			VKContext::get_instance().get_device()->get_vulkan_graphics_queue(),
			VKContext::get_instance().get_vulkan_command_pool());

		vkDestroyBuffer(vkdev, staging_buffer, nullptr);
		vkFreeMemory(vkdev, staging_buffer_mem, nullptr);
	}
}

void VKRenderer2d::_update_matrices()
{

}


/**
* @method VKRenderer2d::_update_commands
* @brief update primary commands
* @remarks
*	- performance sensitive method
*	- get called more than 60 Hz
*/
void VKRenderer2d::_update_commands(float elapsed, int img_index)
{
	const VkCommandBuffer& pri_cmd = _primary_cmds[img_index];

	// reset the primary command buffer
	GRT_CHECK(
		VK_SUCCESS == vkResetCommandBuffer(
			pri_cmd,
			0),
		"failed to reset the primary command buffer");

	// begin recording commands
	VkCommandBufferBeginInfo cmd_beg_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	cmd_beg_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	GRT_CHECK(
		VK_SUCCESS == vkBeginCommandBuffer(
			pri_cmd,
			&cmd_beg_info),
		"failed to begin recording the primary commands");

	// begin render pass
	VkRenderPassBeginInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = _render_pass;
	render_pass_info.framebuffer = _frame_buffers[img_index];
	render_pass_info.renderArea.offset = { 0, 0 };
	render_pass_info.renderArea.extent = _swapchain->get_vulkan_extent();

	VkClearValue clear_clr = { 0.2f, 0.2f, 0.3f, 1.0f };
	render_pass_info.clearValueCount = 1;
	render_pass_info.pClearValues = &clear_clr;

	vkCmdBeginRenderPass(
		pri_cmd,
		&render_pass_info,
		VK_SUBPASS_CONTENTS_INLINE);

	// draw here
	VNode::render_param_t render_param = {};
	render_param.cmd				= pri_cmd;
	render_param.pipeline			= _graphics_pipeline;
	render_param.pipeline_layout	= _graphics_pipeline_layout;
	render_param.fbo_index			= img_index;
	render_param.elapsed			= elapsed;
	

	vkCmdBindDescriptorSets(
		pri_cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_graphics_pipeline_layout,
		0,
		1,
		&_desc_static_sets[img_index],
		0, 
		nullptr);

	//vkCmdBindDescriptorSets(
	//	pri_cmd,
	//	VK_PIPELINE_BIND_POINT_GRAPHICS,
	//	_graphics_pipeline_layout,
	//	0,
	//	1,
	//	&_desc_dynamic_sets[img_index],
	//	1,
	//	0);

	_scene_graph->update(render_param);

	// end render pass
	vkCmdEndRenderPass(pri_cmd);

	// end recording commands
	GRT_CHECK(
		VK_SUCCESS == vkEndCommandBuffer(pri_cmd),
		"failed to end recording the primary commands");
}

/**
* @method VKRenderer2d::update
* @brief update a frame to present to the screen.
* @remarks
*	- performance sensitive method
*	- get called more than 60 Hz
*/
void VKRenderer2d::update(float elapsed)
{
	// get the context
	const VkDevice& vkdev = VKContext::get_instance().get_vulkan_device();
	const std::vector<VkSemaphore>& sp_img_avaliable =
		VKContext::get_instance().get_vulkan_semaphores_image_avaliable();
	const std::vector<VkSemaphore>& sp_rdr_finished =
		VKContext::get_instance().get_vulkan_semaphores_rendering_finished();
	const std::vector<VkFence>& fn_inflight =
		VKContext::get_instance().get_vulkan_fences_inflight();

	const int cur_frmidx = VKContext::get_instance().get_vulkan_current_frame_index();

	// wait for the inflight fence
	vkWaitForFences(vkdev, 1, &fn_inflight[cur_frmidx], VK_TRUE, UINT64_MAX);

	// request for an avaliable image
	uint32_t img_idx;
	VkResult vkres = vkAcquireNextImageKHR(
		vkdev,
		_swapchain->get_vulkan_swapchain(),
		UINT64_MAX,
		sp_img_avaliable[cur_frmidx],
		VK_NULL_HANDLE,
		&img_idx);
	
	// check error
	if (vkres == VK_ERROR_OUT_OF_DATE_KHR)
	{
		VKContext::get_instance().resize();
		return;
	}
	else if (vkres != VK_SUCCESS && vkres != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// update commands
	_update_commands(elapsed, img_idx);

	// prepare submission
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = { sp_img_avaliable[cur_frmidx] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = waitStages;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_primary_cmds[img_idx];

	VkSemaphore signal_semaphores[] = { sp_rdr_finished[cur_frmidx] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	// reset the inflight fence
	vkResetFences(vkdev, 1, &fn_inflight[cur_frmidx]);

	// Submit to queue
	GRT_CHECK(
		VK_SUCCESS == vkQueueSubmit(
			VKContext::get_instance().get_device()->get_vulkan_graphics_queue(),
			1,
			&submit_info,
			fn_inflight[cur_frmidx]),
		"failed to submit the draw command buffer");
	
	// present
	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapchains[] = { _swapchain->get_vulkan_swapchain() };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;

	present_info.pImageIndices = &img_idx;

	vkres = vkQueuePresentKHR(
		VKContext::get_instance().get_device()->get_vulkan_present_queue(),
		&present_info);

	if (vkres == VK_ERROR_OUT_OF_DATE_KHR ||
		vkres == VK_SUBOPTIMAL_KHR ||
		VKContext::get_instance().get_flag_resized())
	{
		VKContext::get_instance().resize();
		VKContext::get_instance().set_flag_resized(false);
		return;
	}
	else if (vkres != VK_SUCCESS)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	VKContext::get_instance().next_frame_index();
}
