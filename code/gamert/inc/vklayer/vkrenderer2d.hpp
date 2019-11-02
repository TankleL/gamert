#pragma once

#include "pre-req.hpp"
#include "vkrenderer.hpp"

class VKRenderer2d : public VKRenderer
{
public:
	VKRenderer2d();

public:
	virtual void init(VKSwapchain* swapchain) override;
	virtual void unint() override;
	virtual void update(float elapsed) override;

private:
	void _create_render_pass();
	void _create_framebuffers();

	void _destroy_render_pass();
	void _destroy_framebuffers();

private:
	std::vector<VkFramebuffer>	_frame_buffers;		// vulkan frame buffer

	VkRenderPass				_render_pass;
	VKSwapchain*				_swapchain;
	bool						_initialized;
};
