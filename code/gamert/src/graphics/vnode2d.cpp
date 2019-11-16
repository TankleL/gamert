#include "vnode2d.hpp"

uint32_t VNode2d::_dirty_clean_bits = 0;

VNode2d::VNode2d()
	: _world(VFMat3E)
	, _position(VFMat3E)
	, _rotation(VFMat3E)
	, _scale(VFMat3E)
	, _dirty_bits(0)
{}

VNode2d::~VNode2d()
{}

void VNode2d::set_poisition_fast(const VFVec2& position)
{
	_dirty_bits = 0;
	_position.set_column<2>(2, position);
}

void VNode2d::set_poisition(const VFVec2& position)
{
	set_poisition_fast(position);
	calculate_world();
}

void VNode2d::set_scale_fast(const VFVec2& scale)
{
	_dirty_bits = 0;
	_scale[0][0] = scale[0];
	_scale[1][1] = scale[1];
}

void VNode2d::set_scale(const VFVec2& scale)
{
	set_scale_fast(scale);
	calculate_world();
}

void VNode2d::clean_dirty_matrix(uint32_t fbo_idx)
{
	_dirty_bits |= 1 << fbo_idx;
}

const VFMat3& VNode2d::get_matrix_world() const
{
	return _world;
}

void VNode2d::set_dirty_clean_bits(uint32_t bits)
{
	_dirty_clean_bits = bits;
}

void VNode2d::calculate_world()
{
	_world = _position * _scale;
}

