#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"
#include "vmatrix.hpp"
#include "vnode.hpp"

class VCamera : VNode
{
public:
	VCamera();
	virtual ~VCamera();

public:
	void set_viewport(
		float min_x,
		float min_y,
		float max_x,
		float max_y);

protected:
	VFMat4	_projection;
	VFVec4	_viewport;
	float	_fov;
	float	_near;
	float	_far;

	bool	_projection_dirty;
};
