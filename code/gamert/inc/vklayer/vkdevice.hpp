#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"

class VKDevice
{
public:
	VKDevice(VkPhysicalDevice);

public:
	void init();
	void uninit();
	
public:
	GRT_VULKAN_FACTOR_GETTER(VkDevice, device, _device);
	GRT_VULKAN_FACTOR_GETTER(VkPhysicalDevice, physical_device, _physical_device);
	GRT_VULKAN_FACTOR_GETTER(VkQueue, graphics_queue, _graphics_queue);
	GRT_VULKAN_FACTOR_GETTER(VkQueue, present_queue, _present_queue);

private:
	VkPhysicalDevice	_physical_device;
	VkDevice			_device;
	VkQueue				_graphics_queue;
	VkQueue				_present_queue;
};

