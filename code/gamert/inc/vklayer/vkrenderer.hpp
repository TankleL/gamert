#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"

class VKSwapchain;
class VKRenderer
{
public:
	virtual void init(VKSwapchain* swapchain) = 0;
	virtual void unint() = 0;
	virtual void update(float elapsed) = 0;
};
