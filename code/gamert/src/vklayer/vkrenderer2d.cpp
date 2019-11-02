#include "vkrenderer2d.hpp"
#include "vkcontext.hpp"
#include "vkutils.hpp"
#include "vvertex.hpp"
#include "resourcesmgr.hpp"

using namespace std;

VKRenderer2d::VKRenderer2d()
	: _render_pass(VK_NULL_HANDLE)
	, _graphics_pipeline(VK_NULL_HANDLE)
	, _graphics_pipeline_layout(VK_NULL_HANDLE)
	, _swapchain(nullptr)
	, _initialized(false)
{}

void VKRenderer2d::init(VKSwapchain* swapchain)
{
	if (!_initialized)
	{
		_swapchain = swapchain;
		_create_render_pass();
		_create_framebuffers();
		_create_graphics_pipeline();
		_create_primary_commandbuffers();
		_initialized = true;
	}
}

void VKRenderer2d::unint()
{
	if (_initialized)
	{
		_destroy_primary_commandbuffers();
		_destroy_graphics_pipeline();
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
	VkDevice vkdev = VKContext::get_instance().get_vulkan_device();
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

	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 0;
	pipeline_layout_info.pushConstantRangeCount = 0;

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
	VkDevice vkdev = VKContext::get_instance().get_vulkan_device();
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

/**
* @method VKRenderer2d::_update_commands
* @brief update primary commands
* @remarks
*	- performance sensitive method
*	- get called more than 60 Hz
*/
void VKRenderer2d::_update_commands(int img_index)
{
	// reset the primary command buffer
	GRT_CHECK(
		VK_SUCCESS == vkResetCommandBuffer(
			_primary_cmds[img_index],
			0),
		"failed to reset the primary command buffer");

	// begin recording commands
	VkCommandBufferBeginInfo cmd_beg_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	cmd_beg_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	GRT_CHECK(
		VK_SUCCESS == vkBeginCommandBuffer(
			_primary_cmds[img_index],
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
		_primary_cmds[img_index],
		&render_pass_info,
		VK_SUBPASS_CONTENTS_INLINE);

	// draw here



	// end render pass
	vkCmdEndRenderPass(_primary_cmds[img_index]);

	// end recording commands
	GRT_CHECK(
		VK_SUCCESS == vkEndCommandBuffer(_primary_cmds[img_index]),
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
	_update_commands(img_idx);

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
