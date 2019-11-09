#pragma once

#include "pre-req.hpp"
#include "vnode.hpp"
#include "vvertex.hpp"

class VNode2d : public VNode
{
public:
	typedef struct _st_render_param2d : public _st_render_param
	{
		VkDescriptorSet		descset_dynamic;
	} render_param2d_t;

public:
	VNode2d();
	virtual ~VNode2d();

private:
	VFVec3		_position;
	VFVec3		_rotation;
	VFVec3		_scale;
};

