#include "vkapp.hpp"

/* ****************************************************************************
|                        [Region] Forward Declaration                         |
**************************************************************************** */

static VKAPI_ATTR VkBool32 VKAPI_CALL __debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* usr_data);


/* ****************************************************************************
|                        [Region] define static vars                          |
**************************************************************************** */

#if defined(DEBUG) || defined(_DEBUG)
bool VKApplication::s_enabled_validation_layer = true;
#else
bool VKApplication::s_enabled_validation_layer = false;
#endif

std::vector<const char*> VKApplication::s_enabled_device_extension =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char*> VKApplication::s_validation_layers = 
{
	 "VK_LAYER_KHRONOS_validation"
};

std::vector<const char*> VKApplication::s_enabled_instance_extension =
{
	"VK_KHR_surface"
#if defined(WIN32)
	,"VK_KHR_win32_surface"
#endif

#if defined(DEBUG) ||defined(_DEBUG)
	, VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
};


/* ****************************************************************************
|                       [Region] define static methods                        |
**************************************************************************** */

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

bool VKApplication::s_is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	return s_find_queue_families(device, surface).is_complete() && s_check_device_extension_support(device);
}

bool VKApplication::s_check_device_extension_support(VkPhysicalDevice device)
{
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

	std::set<std::string> required_extensions(s_enabled_device_extension.begin(), s_enabled_device_extension.end());

	for (const auto& extension : available_extensions) {
		required_extensions.erase(extension.extensionName);
	}

	return required_extensions.empty();
}

VKApplication::QueueFamilyIndices VKApplication::s_find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
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

VKApplication::swapchain_support_details_t 
VKApplication::s_query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
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

VkSurfaceFormatKHR VKApplication::s_choose_swapsurface_format(
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

VkPresentModeKHR VKApplication::s_choose_swap_present_mode(
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

VkExtent2D VKApplication::s_choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actual_extent = { width, height };

		actual_extent.width = max(
			capabilities.minImageExtent.width,
			min(capabilities.maxImageExtent.width, actual_extent.width));
		actual_extent.height = max(
			capabilities.minImageExtent.height,
			min(capabilities.maxImageExtent.height, actual_extent.height));

		return actual_extent;
	}
}


/* ****************************************************************************
|                     [Region] VKApplication Implementation                   |
**************************************************************************** */

#if defined(WIN32)
VKApplication::VKApplication(HWND hwnd) noexcept
	: _hwnd(hwnd)
	, _view_width(1000)
	, _view_height(800)
	, _max_frames_in_flight(2)
	, _cur_frame_idx(0)
	, _vkinst(VK_NULL_HANDLE)
	, _vkdbgmsgr(VK_NULL_HANDLE)
	, _vkphydev(VK_NULL_HANDLE)
	, _vkdevice(VK_NULL_HANDLE)
	, _vkgque(VK_NULL_HANDLE)
	, _vkpque(VK_NULL_HANDLE)
	, _vksrf(VK_NULL_HANDLE)
	, _vkschain(VK_NULL_HANDLE)
	, _vkscimgfmt(VK_FORMAT_UNDEFINED)
	, _vkscext{ 0 }
	, _vkrdrpass(VK_NULL_HANDLE)
	, _vkcmdpool(VK_NULL_HANDLE)
{}
#else
VKApplication::VKApplication() noexcept
	: _hwnd(hwnd)
	, _view_width(1000)
	, _view_height(800)
	, _max_frames_in_flight(2)
	, _cur_frame_idx(0)
	, _vkinst(VK_NULL_HANDLE)
	, _vkdbgmsgr(VK_NULL_HANDLE)
	, _vkphydev(VK_NULL_HANDLE)
	, _vkdevice(VK_NULL_HANDLE)
	, _vkgque(VK_NULL_HANDLE)
	, _vkpque(VK_NULL_HANDLE)
	, _vksrf(VK_NULL_HANDLE)
	, _vkschain(VK_NULL_HANDLE)
	, _vkscimgfmt(VK_FORMAT_UNDEFINED)
	, _vkscext{ 0 }
	, _vkrdrpass(VK_NULL_HANDLE)
	, _vkcmdpool(VK_NULL_HANDLE)
{}
#endif

void VKApplication::init()
{
	_create_instance();
	_setup_debug();
	_create_surface();
	_pick_physical_device();
	_create_logic_device();
	_create_swapchain();
	_create_image_views();
	_create_render_pass();
	_create_frame_buffers();
	_create_cmd_pool();
	_create_cmd_buffers();
	_create_sync_objects();
}

void VKApplication::tick()
{
	_drawframe();
}

void VKApplication::uninit()
{
	// clean up sync objects
	for (size_t i = 0; i < _max_frames_in_flight; ++i)
	{
		vkDestroySemaphore(_vkdevice, _vksp_imgavaliable[i], nullptr);
		vkDestroySemaphore(_vkdevice, _vksp_rdrfinished[i], nullptr);
		vkDestroyFence(_vkdevice, _vkfences_inflight[i], nullptr);
	}

	// clean up command pool
	vkDestroyCommandPool(_vkdevice, _vkcmdpool, nullptr);

	// clean up frame buffers
	for (const auto& fb : _vksc_framebuffers)
	{
		vkDestroyFramebuffer(_vkdevice, fb, nullptr);
	}

	// clean up render pass
	vkDestroyRenderPass(_vkdevice, _vkrdrpass, nullptr);

	// clean up swapchain image views
	for (const auto& iv : _vkscimgviews)
	{
		vkDestroyImageView(_vkdevice, iv, nullptr);
	}

	// clean up swapchain
	vkDestroySwapchainKHR(_vkdevice, _vkschain, nullptr);

	// clean up vk device
	vkDestroyDevice(_vkdevice, nullptr);

	// clean up surface
	vkDestroySurfaceKHR(_vkinst, _vksrf, nullptr);

	// clean up vk debug utils
	if (s_enabled_validation_layer)
	{
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
	vkapp_create_info.enabledExtensionCount = (uint32_t)s_enabled_instance_extension.size();
	vkapp_create_info.ppEnabledExtensionNames = s_enabled_instance_extension.data();

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

void VKApplication::_create_surface()
{
	VkWin32SurfaceCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.hwnd = _hwnd;
	create_info.hinstance = GetModuleHandle(nullptr);

	VkResult vkr = vkCreateWin32SurfaceKHR(_vkinst, &create_info, nullptr, &_vksrf);

	if (vkr != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
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
		if (s_is_device_suitable(device, _vksrf)) {
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
	queue_family_indices_t indices = s_find_queue_families(_vkphydev, _vksrf);

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	std::set<uint32_t> unique_queue_families = { indices.graphics_family, indices.present_family };

	const float queue_priority = 1.0f;
	for (uint32_t qf : unique_queue_families) {
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

	create_info.ppEnabledExtensionNames = s_enabled_device_extension.data();
	create_info.enabledExtensionCount = (uint32_t)s_enabled_device_extension.size();

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

	vkGetDeviceQueue(_vkdevice, indices.graphics_family, 0, &_vkgque);
	vkGetDeviceQueue(_vkdevice, indices.present_family, 0, &_vkpque);
}

void VKApplication::_create_swapchain()
{
	SwapChainSupportDetails swapchain_support = s_query_swapchain_support(_vkphydev, _vksrf);

	VkSurfaceFormatKHR surface_format = s_choose_swapsurface_format(swapchain_support.formats);
	VkPresentModeKHR presentMode = s_choose_swap_present_mode(swapchain_support.presentModes);
	VkExtent2D extent = s_choose_swap_extent(swapchain_support.capabilities, _view_width, _view_height);

	uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
	if (swapchain_support.capabilities.maxImageCount > 0 &&
		image_count > swapchain_support.capabilities.maxImageCount)
	{
		image_count = swapchain_support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = _vksrf;

	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = s_find_queue_families(_vkphydev, _vksrf);
	uint32_t queueFamilyIndices[] = { indices.graphics_family, indices.present_family };

	if (indices.graphics_family != indices.present_family) {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	create_info.preTransform = swapchain_support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = presentMode;
	create_info.clipped = VK_TRUE;

	create_info.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(_vkdevice, &create_info, nullptr, &_vkschain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(_vkdevice, _vkschain, &image_count, nullptr);
	_vkscimgs.resize(image_count);
	vkGetSwapchainImagesKHR(_vkdevice, _vkschain, &image_count, _vkscimgs.data());

	_vkscimgfmt = surface_format.format;
	_vkscext = extent;
}

void VKApplication::_create_image_views()
{
	_vkscimgviews.resize(_vkscimgs.size());

	for (size_t i = 0; i < _vkscimgs.size(); i++) {
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = _vkscimgs[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = _vkscimgfmt;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_vkdevice, &create_info, nullptr, &_vkscimgviews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void VKApplication::_create_render_pass()
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = _vkscimgfmt;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &color_attachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(_vkdevice, &renderPassInfo, nullptr, &_vkrdrpass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void VKApplication::_create_frame_buffers()
{
	_vksc_framebuffers.resize(_vkscimgviews.size());

	for (size_t i = 0; i < _vkscimgviews.size(); ++i)
	{
		VkImageView attachments[] =
		{
			_vkscimgviews[i]
		};

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = _vkrdrpass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = _vkscext.width;
		framebuffer_info.height = _vkscext.height;
		framebuffer_info.layers = 1;

		if (vkCreateFramebuffer(_vkdevice, &framebuffer_info, nullptr, &_vksc_framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VKApplication::_create_cmd_pool()
{
	queue_family_indices_t indices = s_find_queue_families(_vkphydev, _vksrf);

	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = indices.graphics_family;

	if (vkCreateCommandPool(_vkdevice, &pool_info, nullptr, &_vkcmdpool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void VKApplication::_create_cmd_buffers()
{
	_vkcmdbuffers.resize(_vksc_framebuffers.size());

	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = _vkcmdpool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = (uint32_t)_vkcmdbuffers.size();

	if (vkAllocateCommandBuffers(_vkdevice, &alloc_info, _vkcmdbuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < _vkcmdbuffers.size(); i++) {
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(_vkcmdbuffers[i], &begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = _vkrdrpass;
		render_pass_info.framebuffer = _vksc_framebuffers[i];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = _vkscext;

		VkClearValue clear_color = { 0.18f, 0.18f, 0.28f, 1.0f };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		vkCmdBeginRenderPass(_vkcmdbuffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		//vkCmdBindPipeline(_vkcmdbuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		//vkCmdDraw(_vkcmdbuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(_vkcmdbuffers[i]);

		if (vkEndCommandBuffer(_vkcmdbuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void VKApplication::_create_sync_objects()
{
	_vksp_imgavaliable.resize(_max_frames_in_flight);
	_vksp_rdrfinished.resize(_max_frames_in_flight);
	_vkfences_inflight.resize(_max_frames_in_flight);

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < _max_frames_in_flight; ++i)
	{
		if (vkCreateSemaphore(_vkdevice, &semaphore_info, nullptr, &_vksp_imgavaliable[i]) != VK_SUCCESS ||
			vkCreateSemaphore(_vkdevice, &semaphore_info, nullptr, &_vksp_rdrfinished[i]) != VK_SUCCESS ||
			vkCreateFence(_vkdevice, &fence_info, nullptr, &_vkfences_inflight[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void VKApplication::_drawframe()
{
	vkWaitForFences(_vkdevice, 1, &_vkfences_inflight[_cur_frame_idx], VK_TRUE, UINT64_MAX);
	vkResetFences(_vkdevice, 1, &_vkfences_inflight[_cur_frame_idx]);

	uint32_t img_idx;
	vkAcquireNextImageKHR(
		_vkdevice,
		_vkschain,
		UINT64_MAX,
		_vksp_imgavaliable[_cur_frame_idx],
		VK_NULL_HANDLE,
		&img_idx);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = { _vksp_imgavaliable[_cur_frame_idx] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_vkcmdbuffers[img_idx];

	VkSemaphore signalSemaphores[] = { _vksp_rdrfinished[_cur_frame_idx] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(_vkgque, 1, &submit_info, _vkfences_inflight[_cur_frame_idx]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { _vkschain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;

	present_info.pImageIndices = &img_idx;

	vkQueuePresentKHR(_vkpque, &present_info);

	_cur_frame_idx = (_cur_frame_idx + 1) % _max_frames_in_flight;
}

/* ****************************************************************************
|                          [Region] C-Style Functions                         |
**************************************************************************** */

static VKAPI_ATTR VkBool32 VKAPI_CALL __debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* usr_data) {
	std::cerr << "vk validation layer: " << callback_data->pMessage << std::endl;

	return VK_FALSE;
}

