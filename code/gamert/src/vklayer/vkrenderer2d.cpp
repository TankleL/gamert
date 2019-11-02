#include "vkrenderer2d.hpp"
#include "vkcontext.hpp"

using namespace std;

VKRenderer2d::VKRenderer2d()
	: _render_pass(VK_NULL_HANDLE)
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
		_initialized = true;
	}
}

void VKRenderer2d::unint()
{
	if (_initialized)
	{
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
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
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

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &color_attachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	GRT_CHECK(
		VK_SUCCESS == vkCreateRenderPass(
			VKContext::get_instance().get_vulkan_device(),
			&renderPassInfo,
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

/**
@method VKRenderer2d::update
@brief update a frame to present to the screen.
@remarks
	- performance sensitive method
	- get called more than 60 Hz
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

	//// wait for the inflight fence
	//vkWaitForFences(vkdev, 1, &fn_inflight[cur_frmidx], VK_TRUE, UINT64_MAX);

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

	//// reset the inflight fence
	//vkResetFences(vkdev, 1, &fn_inflight[cur_frmidx]);
	
	// present
	VkSemaphore signal_semaphores[] = { sp_rdr_finished[cur_frmidx] };

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
