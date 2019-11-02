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
	
private:
	VkPhysicalDevice	_physical_device;
	VkDevice			_device;
	VkQueue				_graphics_queue;
	VkQueue				_present_queue;
};

