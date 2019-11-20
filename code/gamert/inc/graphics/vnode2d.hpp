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
	void set_poisition(const VFVec2& position);
	void set_poisition_fast(const VFVec2& position);
	void set_scale(const VFVec2& scale);
	void set_scale_fast(const VFVec2& scale);
	void get_position(VFVec2& position);

	void calculate_world();
	void clean_dirty_matrix(uint32_t fbo_idx);

	const VFMat3& get_matrix_world() const;

public:
	static void set_dirty_clean_bits(uint32_t bits);

protected:
	VFMat3		_world;
	VFMat3		_position;
	VFMat3		_rotation;
	VFMat3		_scale;
	uint32_t	_dirty_bits;

protected:
	static uint32_t	_dirty_clean_bits;
};

