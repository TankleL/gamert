#include "vkapp.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL __debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* usr_data);

#if defined(DEBUG) || defined(_DEBUG)
bool VKApplication::s_enabled_validation_layer = true;
#else
bool VKApplication::s_enabled_validation_layer = false;
#endif


std::vector<const char*> VKApplication::s_validation_layers = 
{
	 "VK_LAYER_KHRONOS_validation"
};

VkResult VKApplication::s_create_debug_utils_messenger_ext(
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

void VKApplication::s_destroy_debug_utils_messenger_ext(
	VkInstance vkinst,
	VkDebugUtilsMessengerEXT dbgmsgr,
	const VkAllocationCallbacks* allocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkinst, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vkinst, dbgmsgr, allocator);
	}
}

bool VKApplication::s_check_validation_layer_support()
{
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> availableLayers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, availableLayers.data());

	for (const char* layerName : s_validation_layers) {
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

void VKApplication::s_populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
	create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = __debug_callback;
}

bool VKApplication::s_is_device_suitable(VkPhysicalDevice device)
{
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

	for (const auto& qf : queue_families)
	{
		if (qf.queueCount > 0 && qf.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			return true;
		}
	}

	return false;
}

uint32_t VKApplication::s_get_queue_family_index(VkPhysicalDevice device)
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

VKApplication::VKApplication() noexcept
	: _vkinst(VK_NULL_HANDLE)
	, _vkdbgmsgr(VK_NULL_HANDLE)
	, _vkphydev(VK_NULL_HANDLE)
	, _vkdevice(VK_NULL_HANDLE)
	, _vkgque(VK_NULL_HANDLE)
{}

void VKApplication::init()
{
	_create_instance();
	_setup_debug();
	_pick_physical_device();
	_create_logic_device();
}

void VKApplication::tick()
{}

void VKApplication::uninit()
{
	// clean up vk device
	vkDestroyDevice(_vkdevice, nullptr);

	// clean up vk debug utils
	if (s_enabled_validation_layer) {
		s_destroy_debug_utils_messenger_ext(_vkinst, _vkdbgmsgr, nullptr);
	}

	// clean up vk instance
	vkDestroyInstance(_vkinst, nullptr);
}

void VKApplication::_create_instance()
{
	if (s_enabled_validation_layer && !s_check_validation_layer_support()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo vkapp_info = {};
	vkapp_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkapp_info.pApplicationName = "gamert";
	vkapp_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	vkapp_info.pEngineName = "gamert engine";
	vkapp_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	vkapp_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo vkapp_create_info = {};
	vkapp_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vkapp_create_info.pApplicationInfo = &vkapp_info;

	VkDebugUtilsMessengerCreateInfoEXT vkdebug_create_info;
	if (s_enabled_validation_layer) {
		vkapp_create_info.enabledLayerCount = static_cast<uint32_t>(s_validation_layers.size());
		vkapp_create_info.ppEnabledLayerNames = s_validation_layers.data();

		s_populate_debug_messenger_create_info(vkdebug_create_info);
		vkapp_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& vkdebug_create_info;
	}
	else {
		vkapp_create_info.enabledLayerCount = 0;
		vkapp_create_info.pNext = nullptr;
	}

	if (vkCreateInstance(&vkapp_create_info, nullptr, &_vkinst) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vk instance");
	}
}

void VKApplication::_setup_debug()
{
	if (s_enabled_validation_layer)
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info;
		s_populate_debug_messenger_create_info(create_info);

		if (s_create_debug_utils_messenger_ext(_vkinst, &create_info, nullptr, &_vkdbgmsgr) != VK_SUCCESS) {
			//throw std::runtime_error("failed to set up vk debug messenger!");
			std::cerr << "failed to set up vk debug messenger!" << std::endl;
		}
	}
}

void VKApplication::_pick_physical_device()
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(_vkinst, &device_count, nullptr);

	if (device_count == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(_vkinst, &device_count, devices.data());

	for (const auto& device : devices) {
		if (s_is_device_suitable(device)) {
			_vkphydev = device;
			break;
		}
	}

	if (_vkphydev == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a vulkan suitable GPU!");
	}
}

void VKApplication::_create_logic_device()
{
	uint32_t qfindex = s_get_queue_family_index(_vkphydev);

	VkDeviceQueueCreateInfo queue_create_info = {};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = qfindex;
	queue_create_info.queueCount = 1;

	float queue_priority = 1.0f;
	queue_create_info.pQueuePriorities = &queue_priority;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	create_info.pQueueCreateInfos = &queue_create_info;
	create_info.queueCreateInfoCount = 1;

	create_info.pEnabledFeatures = &deviceFeatures;

	create_info.enabledExtensionCount = 0;

	if (s_enabled_validation_layer) {
		create_info.enabledLayerCount = static_cast<uint32_t>(s_validation_layers.size());
		create_info.ppEnabledLayerNames = s_validation_layers.data();
	}
	else {
		create_info.enabledLayerCount = 0;
	}

	if (vkCreateDevice(_vkphydev, &create_info, nullptr, &_vkdevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(_vkdevice, qfindex, 0, &_vkgque);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL __debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* usr_data) {
	std::cerr << "vk validation layer: " << callback_data->pMessage << std::endl;

	return VK_FALSE;
}

