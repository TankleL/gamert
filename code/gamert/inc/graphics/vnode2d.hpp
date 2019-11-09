#pragma once

#include "pre-req.hpp"
#include "vnode.hpp"
#include "vvertex.hpp"

class VNode2d : public VNode
{
public:
	VNode2d();
	virtual ~VNode2d();

private:
	VFVec3		_position;
	VFVec3		_rotation;
	VFVec3		_scale;
};

