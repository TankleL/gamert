#include "vkcontext.hpp"
#include "vkapp.hpp"
#include "vkutils.hpp"

using namespace std;

VKContext::VKContext()
	: _device(nullptr)
	, _vksrf(VK_NULL_HANDLE)
	, _vkinstance(VK_NULL_HANDLE)
	, _vkdbgmsgr(VK_NULL_HANDLE)
{}

void VKContext::init(
	const std::string& app_name,
	const std::string& engine_name,
	uint32_t app_version,
	uint32_t engine_version,
	void* hwnd)
{

	// create vulkan instance

	VkApplicationInfo vkapp_info = {};
	vkapp_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkapp_info.pApplicationName = app_name.c_str();
	vkapp_info.applicationVersion = app_version;
	vkapp_info.pEngineName = engine_name.c_str();
	vkapp_info.engineVersion = engine_version;
	vkapp_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo vkapp_create_info = {};
	vkapp_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vkapp_create_info.pApplicationInfo = &vkapp_info;
	vkapp_create_info.enabledExtensionCount = (uint32_t)VKUtils::enabled_instance_extension.size();
	vkapp_create_info.ppEnabledExtensionNames = VKUtils::enabled_instance_extension.data();

	VkDebugUtilsMessengerCreateInfoEXT vkdebug_create_info;
	if (VKUtils::enabled_validation_layer)
	{
		vkapp_create_info.enabledLayerCount = static_cast<uint32_t>(VKUtils::validation_layers.size());
		vkapp_create_info.ppEnabledLayerNames = VKUtils::validation_layers.data();

		VKUtils::populate_debug_messenger_create_info(vkdebug_create_info);
		vkapp_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& vkdebug_create_info;
	}
	else
	{
		vkapp_create_info.enabledLayerCount = 0;
		vkapp_create_info.pNext = nullptr;
	}

	GRT_CHECK(
		VK_SUCCESS == vkCreateInstance(
			&vkapp_create_info,
			nullptr,
			&_vkinstance),
		"failed to create vk instance");


	// setup debug

	GRT_CHECK(
		!VKUtils::enabled_validation_layer ||
		(VKUtils::enabled_validation_layer &&
			VKUtils::check_validation_layer_support()),
		"validation layers requested, but not available!");

	if (VKUtils::enabled_validation_layer)
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info;
		VKUtils::populate_debug_messenger_create_info(create_info);

		GRT_CHECK(
			VK_SUCCESS == VKUtils::create_debug_utils_messenger_ext(_vkinstance, &create_info, nullptr, &_vkdbgmsgr),
			"failed to set up vk debug messenger!");
	}

	// create surface

	VkWin32SurfaceCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.hwnd = (HWND)hwnd;
	create_info.hinstance = GetModuleHandle(nullptr);

	GRT_CHECK(
		VK_SUCCESS == vkCreateWin32SurfaceKHR(_vkinstance, &create_info, nullptr, &_vksrf),
		"failed to create window surface.");
	
	// create device
	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VKUtils::choose_physical_device(physical_device, _vkinstance, _vksrf);

	_device = new VKDevice(physical_device);
	_device->init();
}

void VKContext::uninit()
{
	_device->uninit();

	// clean up surface
	vkDestroySurfaceKHR(_vkinstance, _vksrf, nullptr);

	// clean up vk debug utils
	if (VKUtils::enabled_validation_layer)
	{
		VKUtils::destroy_debug_utils_messenger_ext(_vkinstance, _vkdbgmsgr, nullptr);
	}

	// clean up vk instance
	vkDestroyInstance(_vkinstance, nullptr);
}


