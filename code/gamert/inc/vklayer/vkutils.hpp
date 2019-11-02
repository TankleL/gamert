#pragma once

#include "pre-req.hpp"

#include <vulkan/vulkan.hpp>

class VKUtils
{
public:
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

public:
	static VkResult create_debug_utils_messenger_ext(
		VkInstance vkinst,
		const VkDebugUtilsMessengerCreateInfoEXT* create_info,
		const VkAllocationCallbacks* allocator,
		VkDebugUtilsMessengerEXT* dbgmsgr);

	static void destroy_debug_utils_messenger_ext(
		VkInstance vkinst,
		VkDebugUtilsMessengerEXT dbgmsgr,
		const VkAllocationCallbacks* allocator);

	static bool check_validation_layer_support();

	static void populate_debug_messenger_create_info(
		VkDebugUtilsMessengerCreateInfoEXT& create_info);

	static QueueFamilyIndices find_queue_families(
		VkPhysicalDevice device,
		VkSurfaceKHR surface);

	static bool is_device_suitable(
		VkPhysicalDevice device,
		VkSurfaceKHR surface);

	static uint32_t get_queue_family_index(
		VkPhysicalDevice device);

	static bool check_device_extension_support(
		VkPhysicalDevice device);

	static swapchain_support_details_t query_swapchain_support(
		VkPhysicalDevice device,
		VkSurfaceKHR surface);

	static VkSurfaceFormatKHR choose_swapsurface_format(
		const std::vector<VkSurfaceFormatKHR>& available_formats);

	static VkPresentModeKHR choose_swap_present_mode(
		const std::vector<VkPresentModeKHR>& available_present_modes);

	static VkExtent2D choose_swap_extent(
		const VkSurfaceCapabilitiesKHR& capabilities,
		void* hwnd);

	static VkShaderModule create_shader_module(
		VkDevice device,
		const std::vector<std::uint8_t>& code);

	static uint32_t find_memory_type(
		VkPhysicalDevice phy_device,
		uint32_t type_filter,
		VkMemoryPropertyFlags prop_flag);

	static void create_buffer(
		VkBuffer& buffer,
		VkDeviceMemory& buffer_memory,
		VkDevice device,
		VkPhysicalDevice physical_device,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties
	);

	static void create_pipeline();

	static void choose_physical_device(
		VkPhysicalDevice& physical_device,
		VkInstance vulkan_instance,
		VkSurfaceKHR vulkan_surface);

public:
	static std::vector<const char*>	enabled_instance_extension;
	static std::vector<const char*>	enabled_device_extension;
	static bool						enabled_validation_layer;
	static std::vector<const char*> validation_layers;
};