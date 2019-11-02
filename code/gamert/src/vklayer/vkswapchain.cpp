#include "vkswapchain.hpp"
#include "VKUtils.hpp"
#include "vkcontext.hpp"

using namespace std;

VKSwapchain::VKSwapchain()
	: _vkschain(VK_NULL_HANDLE)
	, _vkextent({ 0 })
	, _vkscimgfmt(VK_FORMAT_UNDEFINED)
{}

VKSwapchain::~VKSwapchain()
{
	uninit();
}

void VKSwapchain::init(
	VkPhysicalDevice physical_device,
	VkSurfaceKHR surface,
	void* hwnd)
{
	// create swapchain obj
	VKUtils::SwapChainSupportDetails swapchain_support =
		VKUtils::query_swapchain_support(
			physical_device,
			surface);

	VkSurfaceFormatKHR surface_format =
		VKUtils::choose_swapsurface_format(swapchain_support.formats);
	VkPresentModeKHR present_mode =
		VKUtils::choose_swap_present_mode(swapchain_support.presentModes);
	VkExtent2D extent =
		VKUtils::choose_swap_extent(swapchain_support.capabilities, hwnd);

	uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
	if (swapchain_support.capabilities.maxImageCount > 0 &&
		image_count > swapchain_support.capabilities.maxImageCount)
	{
		image_count = swapchain_support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;

	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VKUtils::QueueFamilyIndices indices = VKUtils::find_queue_families(physical_device, surface);
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
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;

	create_info.oldSwapchain = VK_NULL_HANDLE;

	const VkDevice vkdevice = VKContext::get_instance().get_vulkan_device();

	GRT_CHECK(
		VK_SUCCESS == vkCreateSwapchainKHR(
			vkdevice,
			&create_info,
			nullptr,
			&_vkschain),
		"failed to create swap chain.");

	vkGetSwapchainImagesKHR(vkdevice, _vkschain, &image_count, nullptr);
	_vkscimgs.resize(image_count);
	vkGetSwapchainImagesKHR(vkdevice, _vkschain, &image_count, _vkscimgs.data());

	_vkscimgfmt = surface_format.format;
	_vkextent = extent;

	// create image views
	_vkscimgviews.resize(_vkscimgs.size());

	for (size_t i = 0; i < _vkscimgs.size(); ++i)
	{
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

		GRT_CHECK(
			VK_SUCCESS == vkCreateImageView(
				vkdevice,
				&create_info,
				nullptr,
				&_vkscimgviews[i]),
			"failed to create image views!");
	}
}

void VKSwapchain::uninit()
{
	const VkDevice vkdevice = VKContext::get_instance().get_vulkan_device();

	// clean up swapchain image views
	for (const auto& iv : _vkscimgviews)
	{
		vkDestroyImageView(vkdevice, iv, nullptr);
	}
	_vkscimgviews.clear();

	// clean up swapchain
	vkDestroySwapchainKHR(vkdevice, _vkschain, nullptr);
}

