#include "vkdevice.hpp"
#include "VKUtils.hpp"
#include "vkcontext.hpp"

using namespace std;

VKDevice::VKDevice(VkPhysicalDevice physical_device)
	: _physical_device(physical_device)
	, _device(VK_NULL_HANDLE)
	, _graphics_queue(VK_NULL_HANDLE)
	, _present_queue(VK_NULL_HANDLE)
{}

void VKDevice::init()
{
	VKUtils::queue_family_indices_t indices =
		VKUtils::find_queue_families(
			_physical_device,
			VKContext::get_instance().get_vulkan_surface());

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	std::set<uint32_t> unique_queue_families = { indices.graphics_family, indices.present_family };

	const float queue_priority = 1.0f;
	for (uint32_t qf : unique_queue_families)
	{
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = qf;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_priority;
		queue_create_infos.push_back(queue_create_info);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = (uint32_t)queue_create_infos.size();

	create_info.pEnabledFeatures = &deviceFeatures;

	create_info.ppEnabledExtensionNames = VKUtils::enabled_device_extension.data();
	create_info.enabledExtensionCount = (uint32_t)VKUtils::enabled_device_extension.size();

	if (VKUtils::enabled_validation_layer) {
		create_info.enabledLayerCount = static_cast<uint32_t>(VKUtils::validation_layers.size());
		create_info.ppEnabledLayerNames = VKUtils::validation_layers.data();
	}
	else {
		create_info.enabledLayerCount = 0;
	}

	if (vkCreateDevice(_physical_device, &create_info, nullptr, &_device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(_device, indices.graphics_family, 0, &_graphics_queue);
	vkGetDeviceQueue(_device, indices.present_family, 0, &_present_queue);
}

void VKDevice::uninit()
{
	// clean up vk device
	vkDestroyDevice(_device, nullptr);
	_device = VK_NULL_HANDLE;
}
