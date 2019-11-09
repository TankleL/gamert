#pragma once
#include "pre-req.hpp"
#include "vnode2d.hpp"
#include "vvertex.hpp"

class VNodeQuad2d : public VNode2d
{
public:
	VNodeQuad2d();
	virtual ~VNodeQuad2d();

public:
	virtual void on_init() override;
	virtual void on_create_drawcall(VKRenderer* renderer) override;
	virtual void on_prerender() override {};
	virtual void on_render(const render_param_t& param) override;
	virtual void on_postrender() override {};
	virtual void on_destroy_drawcall() override;
	virtual void on_uninit() override;

public:
	void set_vertices(const std::array<VVertex2DRGB, 4>& vertices);
	void set_indices(const std::array<uint16_t, 6>& indices);

private:
	void _create_vertex_buffer();
	void _create_index_buffer();
	void _destroy_buffers();

private:
	std::array<VVertex2DRGB, 4>		_vertices;
	std::array<uint16_t, 6>			_indices;

	VkBuffer		_vbuffer;
	VkDeviceMemory	_vbuffer_mem;
	VkBuffer		_ibuffer;
	VkDeviceMemory	_ibuffer_mem;
};
