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
	void _create_graphics_pipeline();
	void _create_primary_commandbuffers();

	void _destroy_render_pass();
	void _destroy_framebuffers();
	void _destroy_graphics_pipeline();
	void _destroy_primary_commandbuffers();

	void _update_commands(int img_index);

private:
	std::vector<VkFramebuffer>		_frame_buffers;		// vulkan frame buffer

	VkRenderPass					_render_pass;
	VkPipeline						_graphics_pipeline;
	VkPipelineLayout				_graphics_pipeline_layout;
	std::vector<VkCommandBuffer>	_primary_cmds;
	VKSwapchain*					_swapchain;
	bool							_initialized;
};
