#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "vulkan/vulkan.hpp"

class VKContext : public Singleton<VKContext>
{
	DECL_SINGLETON_CTOR(VKContext);
public:
	void init();
	void uninit();

private:
	void _init_physical_devices();

private:
	VkPhysicalDeviceProperties			_device_properties;
	VkQueueFamilyProperties*			_queue_properties;
	VkPhysicalDeviceMemoryProperties	_memory_properties;
	VkPhysicalDevice*					_physical_devices;
};

