#include "vkcontext.hpp"
#include "vkutils.hpp"
#include "vscenegraph2d.hpp"

using namespace std;

VSceneGraph2d::VSceneGraph2d()
	: _vbuffer(VK_NULL_HANDLE)
	, _vbuffer_mem(VK_NULL_HANDLE)
	, _ibuffer(VK_NULL_HANDLE)
	, _ibuffer_mem(VK_NULL_HANDLE)
{
	_robj._vertices[0] = VVertex2DRGB(VFVec2({ -100.f, -100.f }), VFVec3({ 0.9f, 0.5f, 0.3f }));
	_robj._vertices[1] = VVertex2DRGB(VFVec2({ 100.f, -100.f }), VFVec3({ 0.3f, 0.9f, 0.5f }));
	_robj._vertices[2] = VVertex2DRGB(VFVec2({ 100.f, 100.f }), VFVec3({ 0.5f, 0.3f, 0.9f }));
	_robj._vertices[3] = VVertex2DRGB(VFVec2({ -100.f, 100.f }), VFVec3({ 0.3f, 0.9f, 1.5f }));

	_robj._indices = {0, 1, 2, 2, 3, 0};
}

VSceneGraph2d::~VSceneGraph2d()
{}

VkBuffer VSceneGraph2d::get_vertex_buffer() const
{
	return _vbuffer;
}

VkBuffer VSceneGraph2d::get_index_buffer() const
{
	return _ibuffer;
}

void VSceneGraph2d::init()
{
	_create_vertex_buffers();
	_create_index_buffers();
}

void VSceneGraph2d::uninit()
{
	_destroy_vi_buffers();
}

void VSceneGraph2d::_create_vertex_buffers()
{
	// get context factors
	const VkDevice& vkdev = VKContext::get_instance().get_vulkan_device();
	const VkPhysicalDevice& vkphydev = VKContext::get_instance().get_vulkan_physical_device();

	// prapare staging buffer
	VkDeviceSize size =
		(VkDeviceSize)(sizeof(VVertex2DRGB) * _robj._vertices.size());

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
	memcpy(data, _robj._vertices.data(), size);
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

void VSceneGraph2d::_create_index_buffers()
{
	// get context factors
	const VkDevice& vkdev = VKContext::get_instance().get_vulkan_device();
	const VkPhysicalDevice& vkphydev = VKContext::get_instance().get_vulkan_physical_device();

	// prapare staging buffer
	VkDeviceSize size =
		(VkDeviceSize)(sizeof(uint16_t) * _robj._indices.size());

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
	memcpy(data, _robj._indices.data(), size);
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

void VSceneGraph2d::_destroy_vi_buffers()
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

