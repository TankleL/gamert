#include "vnode-quad2d.hpp"
#include "vkutils.hpp"
#include "vkcontext.hpp"
#include "vdrawcall2d.hpp"
#include "vkrenderer2d.hpp"

VNodeQuad2d::VNodeQuad2d()
	: _vertices({})
	, _indices({})
	, _vbuffer(VK_NULL_HANDLE)
	, _vbuffer_mem(VK_NULL_HANDLE)
	, _ibuffer(VK_NULL_HANDLE)
	, _ibuffer_mem(VK_NULL_HANDLE)
{}

VNodeQuad2d::~VNodeQuad2d()
{
	on_uninit();
}

void VNodeQuad2d::on_init()
{
	_create_vertex_buffer();
	_create_index_buffer();
}

void VNodeQuad2d::on_uninit()
{
	_destroy_buffers();
}

void VNodeQuad2d::set_vertices(const std::array<VVertex2DRGB, 4>& vertices)
{
	_vertices = vertices;
}

void VNodeQuad2d::set_indices(const std::array<uint16_t, 6>& indices)
{
	_indices = indices;
}

void VNodeQuad2d::_create_vertex_buffer()
{
	// get context factors
	const VkDevice& vkdev = VKContext::get_instance().get_vulkan_device();
	const VkPhysicalDevice& vkphydev = VKContext::get_instance().get_vulkan_physical_device();

	// prapare staging buffer
	VkDeviceSize size =
		(VkDeviceSize)(sizeof(VVertex2DRGB) * _vertices.size());

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_mem;
	VKUtils::create_buffer(
		staging_buffer,
		staging_buffer_mem,
		vkdev,
		vkphydev,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// write the data into the staging buffer
	void* data;
	vkMapMemory(
		vkdev,
		staging_buffer_mem,
		0,
		size,
		0,
		&data);
	memcpy(data, _vertices.data(), size);
	vkUnmapMemory(vkdev, staging_buffer_mem);

	// create vertex buffer
	VKUtils::create_buffer(
		_vbuffer,
		_vbuffer_mem,
		vkdev,
		vkphydev,
		size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// submit the staging buffer to the vertex buffer
	VKUtils::copy_buffer(
		_vbuffer,
		staging_buffer,
		size,
		vkdev,
		VKContext::get_instance().get_device()->get_vulkan_graphics_queue(),
		VKContext::get_instance().get_vulkan_command_pool());

	// clean up temporary resources
	vkDestroyBuffer(vkdev, staging_buffer, nullptr);
	vkFreeMemory(vkdev, staging_buffer_mem, nullptr);
}

void VNodeQuad2d::_create_index_buffer()
{
	// get context factors
	const VkDevice& vkdev = VKContext::get_instance().get_vulkan_device();
	const VkPhysicalDevice& vkphydev = VKContext::get_instance().get_vulkan_physical_device();

	// prapare staging buffer
	VkDeviceSize size =
		(VkDeviceSize)(sizeof(uint16_t) * _indices.size());

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_mem;
	VKUtils::create_buffer(
		staging_buffer,
		staging_buffer_mem,
		vkdev,
		vkphydev,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// write the data into the staging buffer
	void* data;
	vkMapMemory(
		vkdev,
		staging_buffer_mem,
		0,
		size,
		0,
		&data);
	memcpy(data, _indices.data(), size);
	vkUnmapMemory(vkdev, staging_buffer_mem);

	// create index buffer
	VKUtils::create_buffer(
		_ibuffer,
		_ibuffer_mem,
		vkdev,
		vkphydev,
		size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// submit the staging buffer to the vertex buffer
	VKUtils::copy_buffer(
		_ibuffer,
		staging_buffer,
		size,
		vkdev,
		VKContext::get_instance().get_device()->get_vulkan_graphics_queue(),
		VKContext::get_instance().get_vulkan_command_pool());

	// clean up temporary resources
	vkDestroyBuffer(vkdev, staging_buffer, nullptr);
	vkFreeMemory(vkdev, staging_buffer_mem, nullptr);
}

void VNodeQuad2d::_destroy_buffers()
{
	const VkDevice& vkdev = VKContext::get_instance().get_vulkan_device();

	if (VK_NULL_HANDLE != _vbuffer)
	{
		vkDestroyBuffer(vkdev, _vbuffer, nullptr);
		vkFreeMemory(vkdev, _vbuffer_mem, nullptr);
		_vbuffer = VK_NULL_HANDLE;
	}
	
	if (VK_NULL_HANDLE != _ibuffer)
	{
		vkDestroyBuffer(vkdev, _ibuffer, nullptr);
		vkFreeMemory(vkdev, _ibuffer_mem, nullptr);
		_ibuffer = VK_NULL_HANDLE;
	}
}

void VNodeQuad2d::on_render(const render_param_t& param)
{
	const render_param2d_t& param2d = static_cast<const render_param2d_t&>(param);
	const VDrawCall2d* dc2d = static_cast<const VDrawCall2d*>(_drawcall);
	const VKRenderer2d* rdr2d = static_cast<const VKRenderer2d*>(_drawcall->get_renderer());

	vkCmdBindPipeline(
		param2d.cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		param2d.pipeline);

	VkBuffer		vbuffers[] = { _vbuffer };
	VkDeviceSize	offsets[] = { 0 };

	vkCmdBindVertexBuffers(
		param2d.cmd,
		0,
		1,
		vbuffers,
		offsets);

	vkCmdBindIndexBuffer(
		param2d.cmd,
		_ibuffer,
		0,
		VK_INDEX_TYPE_UINT16);

	static bool go_right = true;
	static float moved = 0.f;

	if(go_right)
	{
		if (moved > 100.f)
			go_right = false;
		moved += 1.5f;
	}
	else
	{
		if (moved < -100.f)
			go_right = true;
		moved -= 1.5f;
	}

	uint32_t ubo_offsets[] = { dc2d->get_single_dc_ubo_offset() };
	VKRenderer2d::ubuf_single_dc_t* ubo_data = rdr2d->get_single_dc_ubo(
		ubo_offsets[0],
		param2d.fbo_index);

	ubo_data->world[0][0] = moved;

	vkCmdBindDescriptorSets(
		param2d.cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		param2d.pipeline_layout,
		1,
		1,
		&param2d.descset_dynamic,
		1,
		ubo_offsets);

	vkCmdDrawIndexed(
		param2d.cmd,
		(uint32_t)_indices.size(),
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
