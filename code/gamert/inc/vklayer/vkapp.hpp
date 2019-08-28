#pragma once

#if defined(WIN32)
#	include <Windows.h>
#	define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstdint>
#include <set>
#include <algorithm>

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
	// TODO: create graphics pipleline
	void _create_frame_buffers();
	void _create_cmd_pool();
	void _create_cmd_buffers();
	void _create_sync_objects();

	void _recreate_swapchain_related();
	void _clean_up_swapchain();

	void _drawframe();

private:
	typedef struct QueueFamilyIndices {
		QueueFamilyIndices()
			: graphics_family(UINT32_MAX)
			, present_family(UINT32_MAX)
		{}

		uint32_t	graphics_family;
		uint32_t	present_family;

		bool is_complete() {
			return (graphics_family != UINT32_MAX &&
					present_family != UINT32_MAX);
		}
	} queue_family_indices_t;

	typedef struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	} swapchain_support_details_t;

private:
	static VkResult s_create_debug_utils_messenger_ext(
		VkInstance vkinst,
		const VkDebugUtilsMessengerCreateInfoEXT* create_info,
		const VkAllocationCallbacks* allocator,
		VkDebugUtilsMessengerEXT* dbgmsgr);

	static void s_destroy_debug_utils_messenger_ext(
		VkInstance vkinst,
		VkDebugUtilsMessengerEXT dbgmsgr,
		const VkAllocationCallbacks* allocator);

	static bool s_check_validation_layer_support();

	static void s_populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);

	static QueueFamilyIndices s_find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

	static bool s_is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);

	static uint32_t s_get_queue_family_index(VkPhysicalDevice device);

	static bool s_check_device_extension_support(VkPhysicalDevice device);

	static swapchain_support_details_t s_query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface);

	VkSurfaceFormatKHR s_choose_swapsurface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);

	VkPresentModeKHR s_choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);

	VkExtent2D s_choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

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
	VkRenderPass					_vkrdrpass;		// vulkan render pass
	// TODO: graphics pipeline
	std::vector<VkFramebuffer>		_vksc_framebuffers;	// vulkan swapchain framebuffers
	VkCommandPool					_vkcmdpool;
	std::vector<VkCommandBuffer>	_vkcmdbuffers;
	std::vector<VkSemaphore>		_vksp_imgavaliable;	// vulkan semaphores - image avaliable
	std::vector<VkSemaphore>		_vksp_rdrfinished;	// vulkan semaphores - rendering finished
	std::vector<VkFence>			_vkfences_inflight;		// vulkan in-flight fences
		
#if defined(WIN32)
private:
	HWND		_hwnd;
#endif

private:
	static std::vector<const char*>	s_enabled_instance_extension;
	static std::vector<const char*>	s_enabled_device_extension;
	static bool						s_enabled_validation_layer;
	static std::vector<const char*> s_validation_layers;
};


