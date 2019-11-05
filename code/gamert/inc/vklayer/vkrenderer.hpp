#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"

class VKSwapchain;
class VKRenderer
{
public:
	virtual void		init(VKSwapchain* swapchain) = 0;
	virtual void		unint() = 0;
	virtual void		update(float elapsed) = 0;
	virtual VkPipeline			get_vulkan_pipeline() const = 0;
	virtual VkPipelineLayout	get_vulkan_pipeline_layout() const = 0;
	virtual const std::vector<VkDescriptorSet>& get_vulkan_descriptor_set() const = 0;
};
