#include "vnode2d.hpp"

VNode2d::VNode2d()
	: _dirty_matrix(true)
{
	_world[0][0] = 1.f;
	_world[1][1] = 1.f;
	_world[2][2] = 1.f;
}

VNode2d::~VNode2d()
{}

void VNode2d::set_poisition(const VFVec3& position)
{
	_position = position;
	_dirty_matrix = true;
	_world[2][0] = position[0];
	_world[2][1] = position[1];
}

void VNode2d::set_rotation(const VFVec3& rotation)
{}

void VNode2d::set_scale(const VFVec3& scale)
{}

void VNode2d::clean_dirty_matrix()
{
	_dirty_matrix = false;
}

const VFMat3& VNode2d::get_matrix_world() const
{
	return _world;
}

