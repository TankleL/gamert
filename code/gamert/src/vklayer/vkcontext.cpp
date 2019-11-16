#include "vkcontext.hpp"
#include "vkapp.hpp"
#include "vkutils.hpp"

using namespace std;

VKContext::VKContext()
	: _vksrf(VK_NULL_HANDLE)
	, _vkinstance(VK_NULL_HANDLE)
	, _vkdbgmsgr(VK_NULL_HANDLE)
	, _vkcmdpool(VK_NULL_HANDLE)
	, _hwnd(nullptr)
	, _device(nullptr)
	, _swapchain(nullptr)
	, _max_frames_in_flight(0)
	, _cur_frame(0)
	, _flag_resized(false)
{}

void VKContext::init(
	const std::string& app_name,
	const std::string& engine_name,
	uint32_t app_version,
	uint32_t engine_version,
	void* hwnd,
	int max_frames_in_flight)
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
	_hwnd = hwnd;
	
	// create device
	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VKUtils::choose_physical_device(physical_device, _vkinstance, _vksrf);

	_device = new VKDevice(physical_device);
	_device->init();

	// create sync objects
	_max_frames_in_flight = max_frames_in_flight;
	const VkDevice& vkdev = _device->get_vulkan_device();
	_sp_imgavaliable.resize(_max_frames_in_flight);
	_sp_rdrfinished.resize(_max_frames_in_flight);
	_fn_inflight.resize(_max_frames_in_flight);

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	for (size_t i = 0; i < _max_frames_in_flight; ++i)
	{
		GRT_CHECK(
			vkCreateSemaphore(vkdev, &semaphore_info, nullptr, &_sp_imgavaliable[i]) == VK_SUCCESS &&
			vkCreateSemaphore(vkdev, &semaphore_info, nullptr, &_sp_rdrfinished[i]) == VK_SUCCESS &&
			vkCreateFence(vkdev, &fence_info, nullptr, &_fn_inflight[i]) == VK_SUCCESS,
			"failed to create synchronization objects for a frame.");
	}

	// create command pool
	VKUtils::queue_family_indices_t indices =
		VKUtils::find_queue_families(
			physical_device,
			_vksrf);

	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = indices.graphics_family;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	GRT_CHECK(
		VK_SUCCESS == vkCreateCommandPool(
			_device->get_vulkan_device(),
			&pool_info,
			nullptr,
			&_vkcmdpool),
		"failed to create command pool.");

	// get basic infos
	vkGetPhysicalDeviceProperties(_device->get_vulkan_physical_device(), &_phydev_prop);
}

void VKContext::destroy()
{
	_swapchain->uninit();

	// clean up synchronous objects
	for (auto& sp : _sp_imgavaliable)
	{
		vkDestroySemaphore(_device->get_vulkan_device(), sp, nullptr);
	}
	_sp_imgavaliable.clear();

	for (auto& sp : _sp_rdrfinished)
	{
		vkDestroySemaphore(_device->get_vulkan_device(), sp, nullptr);
	}
	_sp_rdrfinished.clear();

	for (auto& fn : _fn_inflight)
	{
		vkDestroyFence(_device->get_vulkan_device(), fn, nullptr);
	}
	_fn_inflight.clear();

	// clean up command pool
	if (VK_NULL_HANDLE != _vkcmdpool)
	{
		vkDestroyCommandPool(_device->get_vulkan_device(), _vkcmdpool, nullptr);
		_vkcmdpool = VK_NULL_HANDLE;
	}

	// clean up surface
	if (VK_NULL_HANDLE != _vksrf)
	{
		vkDestroySurfaceKHR(_vkinstance, _vksrf, nullptr);
		_vksrf = VK_NULL_HANDLE;
	}

	// clean up device
	_device->uninit();

	// clean up vk debug utils
	if (VKUtils::enabled_validation_layer && VK_NULL_HANDLE != _vkdbgmsgr)
	{
		VKUtils::destroy_debug_utils_messenger_ext(_vkinstance, _vkdbgmsgr, nullptr);
		_vkdbgmsgr = VK_NULL_HANDLE;
	}

	// clean up vk instance
	if (VK_NULL_HANDLE != _vkinstance)
	{
		vkDestroyInstance(_vkinstance, nullptr);
		_vkinstance = VK_NULL_HANDLE;
	}
}

void VKContext::register_renderer(VKRenderer* renderer)
{
	_renderers.push_back(renderer);
}

void VKContext::unregister_all_renderers()
{
	_renderers.clear();
}

void VKContext::resize()
{
	if (_device)
	{
		vkDeviceWaitIdle(_device->get_vulkan_device());

		bool need_cleanup = _swapchain != nullptr;

		GRT_SAFE_DELETE(_swapchain);
		_swapchain = new VKSwapchain();
		_swapchain->init(
			_device->get_vulkan_physical_device(),
			_vksrf,
			_hwnd);

		for (auto rdr : _renderers)
		{
			if (need_cleanup)	rdr->unint();
			rdr->init(_swapchain);
		}
	}
}

void VKContext::wait_device_idle()
{
	if(_device)
		vkDeviceWaitIdle(_device->get_vulkan_device());
}

