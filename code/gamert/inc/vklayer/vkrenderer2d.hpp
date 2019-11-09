#pragma once

#include "pre-req.hpp"
#include "vmatrix.hpp"
#include "vkrenderer.hpp"
#include "vscenegraph.hpp"
#include "vcamera.hpp"

class VKRenderer2d : public VKRenderer
{
public:
	VKRenderer2d();

public:
	virtual void		init(VKSwapchain* swapchain) override;
	virtual void		unint() override;
	virtual void		update(float elapsed) override;

public:
	void set_scene_graph(VSceneGraph* graph);
	void set_camera(VCamera* camera);

private:
	typedef struct _st_ubuf_stable
	{
		VFVec2		extent;
	} _ubuf_stable_t;

	typedef struct _st_ubuf_single_dc
	{
		alignas(16)	VFMat3		world;
	} _ubuf_single_dc_t;

	typedef struct _st_ubuf_combined_dc
	{
		alignas(16)	 VFMat3		view;
	} _ubuf_combined_dc_t;

private:
	void _create_render_pass();
	void _create_framebuffers();
	void _create_graphics_pipeline();
	void _create_uniform_buffers();
	void _create_descriptor_pool();
	void _create_descriptor_set_layout();
	void _create_descriptor_set();
	void _create_primary_commandbuffers();
	void _ensure_stable_uniform_data();
	

	void _destroy_render_pass();
	void _destroy_framebuffers();
	void _destroy_graphics_pipeline();
	void _destroy_uniform_buffers();
	void _destroy_descriptor_pool();
	void _destroy_descriptor_set_layout();
	void _destroy_descriptor_set();
	void _destroy_primary_commandbuffers();

	void _update_matrices();
	void _update_commands(float elapsed, int img_index);

private:
	enum UBufType : int
	{
		UBT_StableInfo = 0,
		UBT_CombinedDC,
		UBT_SingleDC,

		// <------ insert new types above here.
		UBT_Counts
	};

private:
	std::vector<VkFramebuffer>		_frame_buffers;		// vulkan frame buffer
	std::vector<VkCommandBuffer>	_primary_cmds;
	std::vector<VkBuffer>			_ubuf_stable;			// belongs to static uniform buffer
	std::vector<VkDeviceMemory>		_ubuf_stable_mem;		// belongs to static uniform buffer
	std::vector<VkBuffer>			_ubuf_combined_dc;		// belongs to static uniform buffer
	std::vector<VkDeviceMemory>		_ubuf_combined_dc_mem;	// belongs to static uniform buffer
	std::vector<VkBuffer>			_ubuf_single_dc;		// belongs to dynamic uniform buffer
	std::vector<VkDeviceMemory>		_ubuf_single_dc_mem;	// belongs to dynamic uniform buffer
	std::vector<VkDescriptorSet>	_desc_static_sets;
	std::vector<VkDescriptorSet>	_desc_dynamic_sets;
	VkRenderPass					_render_pass;
	VkPipeline						_graphics_pipeline;
	VkPipelineLayout				_graphics_pipeline_layout;
	VkDescriptorPool				_desc_pool;
	VkDescriptorSetLayout			_desc_static_sl;		// vulkan descriptor set layout
	VkDescriptorSetLayout			_desc_dynamic_sl;		// vulkan descriptor set layout

private:
	VSceneGraph*					_scene_graph;
	VKSwapchain*					_swapchain;
	VCamera*						_camera;
	bool							_initialized;

private:
	static VSceneGraph				_dummy_graph;

};
