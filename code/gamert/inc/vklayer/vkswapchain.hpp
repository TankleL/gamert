#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"
#include "vkdevice.hpp"
#include "vkrenderer.hpp"

class VKSwapchain
{
public:
	VKSwapchain();
	~VKSwapchain();

public:
	void init(
		VkPhysicalDevice physical_device,
		VkSurfaceKHR surface,
		void* hwnd);
	void uninit();

public:
	GRT_VULKAN_FACTOR_GETTER(VkFormat, image_format, _vkscimgfmt);
	GRT_VULKAN_FACTOR_GETTER(const std::vector<VkImageView>&, image_views, _vkscimgviews);
	GRT_VULKAN_FACTOR_GETTER(const VkExtent2D&, extent, _vkextent);
	GRT_VULKAN_FACTOR_GETTER(VkSwapchainKHR, swapchain, _vkschain);

private:
	VkSwapchainKHR				_vkschain;		// vulkan swapchain
	VkExtent2D					_vkextent;		// vulkan extent
	std::vector<VkImage>		_vkscimgs;		// vulkan swapchain images
	VkFormat					_vkscimgfmt;	// vulkan swapchain image format
	std::vector<VkImageView>	_vkscimgviews;	// vulkan swapchain image views
};

