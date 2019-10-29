#include "vkutils.hpp"
#include "vkapp.hpp"

using namespace std;

VKApplication::VKApplication()
	: _vkinstance(VK_NULL_HANDLE)
	, _vkdbgmsgr(VK_NULL_HANDLE)
{}

void VKApplication::init(
	const std::string& app_name,
	const std::string& engine_name,
	uint32_t app_version,
	uint32_t engine_version)
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
	vkapp_create_info.enabledExtensionCount = (uint32_t)VkUtils::enabled_instance_extension.size();
	vkapp_create_info.ppEnabledExtensionNames = VkUtils::enabled_instance_extension.data();

	VkDebugUtilsMessengerCreateInfoEXT vkdebug_create_info;
	if (VkUtils::enabled_validation_layer)
	{
		vkapp_create_info.enabledLayerCount = static_cast<uint32_t>(VkUtils::validation_layers.size());
		vkapp_create_info.ppEnabledLayerNames = VkUtils::validation_layers.data();

		VkUtils::populate_debug_messenger_create_info(vkdebug_create_info);
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
		!VkUtils::enabled_validation_layer ||
		(VkUtils::enabled_validation_layer &&
		VkUtils::check_validation_layer_support()),
		"validation layers requested, but not available!");

	if (VkUtils::enabled_validation_layer)
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info;
		VkUtils::populate_debug_messenger_create_info(create_info);

		GRT_CHECK(
			VK_SUCCESS == VkUtils::create_debug_utils_messenger_ext(_vkinstance, &create_info, nullptr, &_vkdbgmsgr),
			"failed to set up vk debug messenger!");
	}
}
