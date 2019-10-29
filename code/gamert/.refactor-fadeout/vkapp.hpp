#pragma once

#include "pre-req.hpp"
#include <vulkan/vulkan.hpp>

class VKApplication
{
public:
#ifdef WIN32
	VKApplication(HWND hwnd) noexcept;
#else
	VKApplication() noexcept;
#endif

public:
	void init();

	void tick();
	void uninit();

	void on_view_resized();

private:
	void _create_instance();
	void _setup_debug();
	void _create_surface();
	void _pick_physical_device();
	void _create_logic_device();
	void _create_swapchain();
	void _create_image_views();
	void _create_render_pass();
	void _create_frame_buffers();
	void _create_cmd_pool();
	void _create_cmd_buffers();
	void _create_sync_objects();

	void _recreate_swapchain_related();
	void _clean_up_swapchain();

	void _drawframe();

private:

	
private:
	uint32_t						_max_frames_in_flight;
	size_t							_cur_frame_idx;
	bool							_swapchain_recreated;

	VkInstance						_vkinst;		// vulkan instance
	VkDebugUtilsMessengerEXT		_vkdbgmsgr;		// vulkan debug messenger
	VkPhysicalDevice				_vkphydev;		// vulkan physical device
	VkDevice						_vkdevice;
	VkQueue							_vkgque;		// vulkan graphics queue
	VkQueue							_vkpque;		// vulkan present queue
	VkSurfaceKHR					_vksrf;			// vulkan surface
	VkSwapchainKHR					_vkschain;		// vulkan swapchain
	std::vector<VkImage>			_vkscimgs;		// vulkan swapchain images
	VkFormat						_vkscimgfmt;	// vulkan swapchain image format
	VkExtent2D						_vkscext;		// vulkan swapchain extent 2D
	std::vector<VkImageView>		_vkscimgviews;	// vulkan swapchain image views
	
	std::vector<VkFramebuffer>		_vksc_framebuffers;	// vulkan swapchain framebuffers
	VkCommandPool					_vkcmdpool;
	std::vector<VkCommandBuffer>	_vkcmdbuffers;
	std::vector<VkSemaphore>		_vksp_imgavaliable;	// vulkan semaphores - image avaliable
	std::vector<VkSemaphore>		_vksp_rdrfinished;	// vulkan semaphores - rendering finished
	std::vector<VkFence>			_vkfences_inflight;	// vulkan in-flight fences
		
#if defined(WIN32)
private:
	HWND		_hwnd;
#endif
};


