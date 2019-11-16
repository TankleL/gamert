#pragma once

#include "pre-req.hpp"
#include "vscenegraph.hpp"
#include "vvertex.hpp"

class VKRenderer;
class VSceneGraph2d : public VSceneGraph
{
public:
	typedef struct _robject
	{
		std::array<VVertex2DRGB, 4>		_vertices;
		std::array<uint16_t, 6>			_indices;
	} robject_t;	// rendering object.

public:
	VSceneGraph2d();
	virtual ~VSceneGraph2d();

public:
	void	init();
	void	uninit();

	VkBuffer	get_vertex_buffer() const;
	VkBuffer	get_index_buffer() const;

private:
	void _create_vertex_buffers();
	void _create_index_buffers();
	void _destroy_vi_buffers();

private:
	robject_t		_robj;
	VkBuffer		_vbuffer;
	VkDeviceMemory	_vbuffer_mem;
	VkBuffer		_ibuffer;
	VkDeviceMemory	_ibuffer_mem;
};

