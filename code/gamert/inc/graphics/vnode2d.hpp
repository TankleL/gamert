#pragma once

#include "pre-req.hpp"
#include "vnode.hpp"
#include "vvertex.hpp"
#include "vmatrix.hpp"

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

public:
	void set_poisition(const VFVec3& position);
	void set_rotation(const VFVec3& rotation);
	void set_scale(const VFVec3& scale);
	void clean_dirty_matrix();

	const VFMat3& get_matrix_world() const;

protected:
	VFMat3		_world;
	VFVec3		_position;
	VFVec3		_rotation;
	VFVec3		_scale;
	bool		_dirty_matrix;
};

