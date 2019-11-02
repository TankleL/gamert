#include "VKUtils.hpp"



#if defined(DEBUG) || defined(_DEBUG)
bool VKUtils::enabled_validation_layer = true;
#else
bool VKUtils::enabled_validation_layer = false;
#endif

std::vector<const char*> VKUtils::enabled_device_extension =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char*> VKUtils::validation_layers =
{
	 "VK_LAYER_KHRONOS_validation"
};

std::vector<const char*> VKUtils::enabled_instance_extension =
{
	"VK_KHR_surface"
#if defined(WIN32)
	,"VK_KHR_win32_surface"
#endif

#if defined(DEBUG) ||defined(_DEBUG)
	, VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
};

static VKAPI_ATTR VkBool32 VKAPI_CALL __debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* usr_data)
{
	std::ostringstream oss;
	oss << ">>>>>> vk validation layer <<<<<<<\n\t" << callback_data->pMessage << std::endl;

#if defined(_WIN32) || defined(WIN32)
	OutputDebugStringA(oss.str().c_str());
#endif // _WIN32 


	return VK_FALSE;
}

VkResult VKUtils::create_debug_utils_messenger_ext(
	VkInstance vkinst,
	const VkDebugUtilsMessengerCreateInfoEXT* create_info,
	const VkAllocationCallbacks* allocator,
	VkDebugUtilsMessengerEXT* dbgmsgr)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkinst, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(vkinst, create_info, allocator, dbgmsgr);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VKUtils::destroy_debug_utils_messenger_ext(
	VkInstance vkinst,
	VkDebugUtilsMessengerEXT dbgmsgr,
	const VkAllocationCallbacks* allocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkinst, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vkinst, dbgmsgr, allocator);
	}
}

bool VKUtils::check_validation_layer_support()
{
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> availableLayers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, availableLayers.data());

	for (const char* layerName : validation_layers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void VKUtils::populate_debug_messenger_create_info(
	VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
	create_info = {};

	create_info.sType =
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = __debug_callback;
}

bool VKUtils::is_device_suitable(
	VkPhysicalDevice device,
	VkSurfaceKHR surface)
{
	return find_queue_families(device, surface).is_complete() && check_device_extension_support(device);
}

bool VKUtils::check_device_extension_support(
	VkPhysicalDevice device)
{
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

	std::set<std::string> required_extensions(enabled_device_extension.begin(), enabled_device_extension.end());

	for (const auto& extension : available_extensions) {
		required_extensions.erase(extension.extensionName);
	}

	return required_extensions.empty();
}

VKUtils::QueueFamilyIndices VKUtils::find_queue_families(
	VkPhysicalDevice device,
	VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queueFamilies.data());

	int i = 0;
	for (const auto& qf : queueFamilies) {
		if (qf.queueCount > 0 && qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphics_family = i;
		}

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

		if (qf.queueCount > 0 && present_support) {
			indices.present_family = i;
		}

		if (indices.is_complete()) {
			break;
		}

		i++;
	}

	return indices;
}

uint32_t VKUtils::get_queue_family_index(
	VkPhysicalDevice device)
{
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

	uint32_t idx = 0;
	for (const auto& qf : queue_families)
	{
		if (qf.queueCount > 0 && qf.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			break;
		}
		++idx;
	}

	return idx;
}

VKUtils::swapchain_support_details_t
VKUtils::query_swapchain_support(
	VkPhysicalDevice device,
	VkSurfaceKHR surface)
{
	swapchain_support_details_t details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

	if (format_count != 0)
	{
		details.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

	if (present_mode_count != 0)
	{
		details.presentModes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR VKUtils::choose_swapsurface_format(
	const std::vector<VkSurfaceFormatKHR>& available_formats)
{
	for (const auto& af : available_formats)
	{
		if (af.format == VK_FORMAT_B8G8R8A8_UNORM &&
			af.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return af;
		}
	}
	return available_formats[0];
}

VkPresentModeKHR VKUtils::choose_swap_present_mode(
	const std::vector<VkPresentModeKHR>& available_present_modes)
{
	for (const auto& am : available_present_modes)
	{
		if (am == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return am;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VKUtils::choose_swap_extent(
	const VkSurfaceCapabilitiesKHR& capabilities,
	void* hwnd)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width = 0;
		int height = 0;

#if defined(WIN32)
		::RECT client_rect;
		if (::GetClientRect((HWND)hwnd, &client_rect))
		{
			width = client_rect.right - client_rect.left;
			height = client_rect.bottom - client_rect.top;
		}
		else
		{
			throw std::runtime_error("failed to get view size; GetClientRect returns errored!");
		}
#else
		Compile error !
			TODO: Support getting view size !
#endif
			VkExtent2D actual_extent = { (uint32_t)width, (uint32_t)height };

		actual_extent.width = max(
			capabilities.minImageExtent.width,
			min(capabilities.maxImageExtent.width, actual_extent.width));
		actual_extent.height = max(
			capabilities.minImageExtent.height,
			min(capabilities.maxImageExtent.height, actual_extent.height));

		return actual_extent;
	}
}

VkShaderModule VKUtils::create_shader_module(
	VkDevice device,
	const std::vector<std::uint8_t>& code)
{
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = code.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(&code[0]);

	VkShaderModule shader_module;
	if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shader_module;
}

uint32_t VKUtils::find_memory_type(
	VkPhysicalDevice phy_device,
	uint32_t type_filter,
	VkMemoryPropertyFlags prop_flag)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(phy_device, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if ((type_filter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & prop_flag) == prop_flag)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VKUtils::create_buffer(
	VkBuffer& buffer,
	VkDeviceMemory& buffer_memory,
	VkDevice device,
	VkPhysicalDevice physical_device,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties
)
{
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vertex buffer");
	}

	VkMemoryRequirements mem_req;
	vkGetBufferMemoryRequirements(device, buffer, &mem_req);

	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_req.size;
	alloc_info.memoryTypeIndex = find_memory_type(
		physical_device,
		mem_req.memoryTypeBits,
		properties);

	if (vkAllocateMemory(device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device, buffer, buffer_memory, 0);
}

void VKUtils::choose_physical_device(
	VkPhysicalDevice& physical_device,
	VkInstance vulkan_instance,
	VkSurfaceKHR vulkan_surface)
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(vulkan_instance, &device_count, nullptr);

	if (device_count == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(vulkan_instance, &device_count, devices.data());

	for (const auto& device : devices) {
		if (is_device_suitable(device, vulkan_surface)) {
			physical_device = device;
			break;
		}
	}

	GRT_CHECK(
		VK_NULL_HANDLE != physical_device,
		"failed to find a vulkan suitable GPU.");
}
