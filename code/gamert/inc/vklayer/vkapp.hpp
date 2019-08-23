#pragma once

#include <vulkan/vulkan.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <optional>
#include <cstdint>

class VKApplication
{
public:
	VKApplication() noexcept;

public:
	void init();
	void tick();
	void uninit();

private:
	void _create_instance();
	void _setup_debug();
	void _pick_physical_device();
	void _create_logic_device();
	
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

	static bool s_is_device_suitable(VkPhysicalDevice device);

	static uint32_t s_get_queue_family_index(VkPhysicalDevice device);

private:
	VkInstance					_vkinst;		// vulkan instance
	VkDebugUtilsMessengerEXT	_vkdbgmsgr;		// vulkan debug messenger
	VkPhysicalDevice			_vkphydev;		// vulkan physical device
	VkDevice					_vkdevice;
	VkQueue						_vkgque;		// vulkan graphics queue

private:
	static bool						s_enabled_validation_layer;
	static std::vector<const char*> s_validation_layers;
};


