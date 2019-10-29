#pragma once

#include "pre-req.hpp"
#include "vnode.hpp"
#include "vvertex.hpp"

class VNodeRect : public VNode
{
public:
	//virtual void on_init() override;
	//virtual void on_render(std::vector<VkCommandBuffer>& cmdqueue, VkFramebuffer frmbuffer, VkCommandPool cmdpool) override;

protected:
	VVertex2DRGB	_vertices[3];
	VkBuffer		_vertex_buffer;
	VkDeviceMemory	_vertex_dev_mem;
	VkCommandBuffer	_cmd;
};


