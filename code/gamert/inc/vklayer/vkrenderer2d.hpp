#pragma once

#include "pre-req.hpp"
#include "vmatrix.hpp"
#include "vkrenderer.hpp"
#include "vscenegraph.hpp"

class VKRenderer2d : public VKRenderer
{
public:
	VKRenderer2d();

public:
	virtual void		init(VKSwapchain* swapchain) override;
	virtual void		unint() override;
	virtual void		update(float elapsed) override;

	virtual VkPipeline				get_vulkan_pipeline() const override;
	virtual VkPipelineLayout		get_vulkan_pipeline_layout() const override;
	virtual const std::vector<VkDescriptorSet>&	get_vulkan_descriptor_set() const override;

public:
	void set_scene_graph(VSceneGraph* graph);

private:
	typedef struct _st_uniform_buffer_object
	{
		alignas(16)		VFMat4 model;
		alignas(16)		VFMat4 view;
		alignas(16)		VFMat4 proj;
	} _uniform_buffer_object_t;

private:
	void _create_render_pass();
	void _create_framebuffers();
	void _create_graphics_pipeline();
	void _create_uniform_buffers();
	void _create_descriptor_pool();
	void _create_descriptor_set_layout();
	void _create_descriptor_set();
	void _create_primary_commandbuffers();
	

	void _destroy_render_pass();
	void _destroy_framebuffers();
	void _destroy_graphics_pipeline();
	void _destroy_uniform_buffers();
	void _destroy_descriptor_pool();
	void _destroy_descriptor_set_layout();
	void _destroy_descriptor_set();
	void _destroy_primary_commandbuffers();

	void _update_commands(float elapsed, int img_index);

private:
	std::vector<VkFramebuffer>		_frame_buffers;		// vulkan frame buffer
	std::vector<VkCommandBuffer>	_primary_cmds;
	std::vector<VkBuffer>			_uniform_buffers;
	std::vector<VkDeviceMemory>		_uniform_buffer_mems;
	std::vector<VkDescriptorSet>	_descriptor_sets;
	VkRenderPass					_render_pass;
	VkPipeline						_graphics_pipeline;
	VkPipelineLayout				_graphics_pipeline_layout;
	VkDescriptorPool				_descriptor_pool;
	VkDescriptorSetLayout			_descriptor_set_layout;

private:
	VSceneGraph*					_scene_graph;
	VKSwapchain*					_swapchain;
	bool							_initialized;

private:
	static VSceneGraph				_dummy_graph;

};
