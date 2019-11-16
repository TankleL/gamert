#include "vnode-quad2d.hpp"
#include "vkutils.hpp"
#include "vkcontext.hpp"
#include "vdrawcall2d.hpp"
#include "vkrenderer2d.hpp"

VNodeQuad2d::VNodeQuad2d()
{}

VNodeQuad2d::~VNodeQuad2d()
{
	on_uninit();
}

void VNodeQuad2d::on_init()
{}

void VNodeQuad2d::on_uninit()
{}

void VNodeQuad2d::on_render(const render_param_t& param)
{
	const render_param2d_t& param2d = static_cast<const render_param2d_t&>(param);
	const VDrawCall2d* dc2d = static_cast<const VDrawCall2d*>(_drawcall);
	const VKRenderer2d* rdr2d = static_cast<const VKRenderer2d*>(_drawcall->get_renderer());

	uint32_t ubo_offset =
		dc2d->get_single_dc_ubo_offset() *
		(uint32_t)VKContext::get_instance().get_vulkan_min_uniform_buffer_offset_alignment();
	VKRenderer2d::ubuf_single_dc_t* ubo_data = rdr2d->get_single_dc_ubo(
		ubo_offset,
		param2d.fbo_index);
	
	//if (_dirty_matrix)
	{
		memcpy(
			&(ubo_data->world),
			&_world,
			sizeof(VFMat3));
		clean_dirty_matrix();
	}

	vkCmdBindDescriptorSets(
		param2d.cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		param2d.pipeline_layout,
		1,
		1,
		&param2d.descset_dynamic,
		1,
		&ubo_offset);

	vkCmdDrawIndexed(
		param2d.cmd,
		6,
		1,
		0,
		0,
		0);
}

void VNodeQuad2d::on_create_drawcall(VKRenderer* renderer)
{
	_drawcall = new VDrawCall2d(renderer);
}

void VNodeQuad2d::on_destroy_drawcall()
{
	GRT_SAFE_DELETE(_drawcall);
}
