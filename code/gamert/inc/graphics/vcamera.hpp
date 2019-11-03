#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"
#include "vmatrix.hpp"

class VCamera
{
public:


protected:
	VFMat4	_projection;
	VFVec4	_viewport;
	float	_fov;
	float	_near;
	float	_far;

	bool	_projection_dirty;
};
