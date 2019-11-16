#pragma once

#include "pre-req.hpp"
#include "vmatrix.hpp"
#include "vkrenderer.hpp"
#include "vscenegraph2d.hpp"
#include "vcamera.hpp"

class VKRenderer2d : public VKRenderer
{
public:
	typedef struct _st_ubuf_single_dc
	{
		alignas(16) VFMat3		world;
	} ubuf_single_dc_t;

public:
	VKRenderer2d(uint32_t max_drawcalls = 128);

public:
	virtual void		init(VKSwapchain* swapchain) override;
	virtual void		unint() override;
	virtual void		update(float elapsed) override;

public:
	void				bind_scene_graph(VSceneGraph2d* graph);
	void				unbind_scene_graph();
	void				set_camera(VCamera* camera);
	uint32_t			allocate_single_dc_ubo();
	void				free_single_dc_ubo(uint32_t offset);
	ubuf_single_dc_t*	get_single_dc_ubo(uint32_t offset, int fbo_index) const;

private:
	typedef struct _st_ubuf_stable
	{
		VFVec2	extent;
	} _ubuf_stable_t;

	typedef struct _st_ubuf_combined_dc
	{
		VFMat3	view;
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
	void _reset_single_dc_marks();
	void _reset_vnode2d_dirty_clean_bits();


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
	std::vector<void*>				_mapped_data_single_dc;
	std::vector<bool>				_single_dc_marks;		// manage the allocation of single dc buffer
	uint32_t						_single_de_buf_alignment;
	VSceneGraph2d*					_scene_graph;
	VKSwapchain*					_swapchain;
	VCamera*						_camera;
	const uint32_t					_max_drawcalls;
	bool							_initialized;

private:
	static VSceneGraph2d			_dummy_graph;

};
