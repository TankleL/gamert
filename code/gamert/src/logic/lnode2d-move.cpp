#include "logicmgr.hpp"
#include "lnode2d-move.hpp"
#include "joystick.hpp"

REG_LND_CREATOR(LNode2dMove, "Node2d-Move");

LNode2dMove::LNode2dMove()
	: _vnode(nullptr)
	, _controller(0)
{}

LNode2dMove::~LNode2dMove()
{}

void LNode2dMove::bind(VNode2d* node)
{
	_vnode = node;
}

void LNode2dMove::set_controller(int controller)
{
	_controller = controller;
}

void LNode2dMove::on_tick(const tick_param_t& param)
{
	if (_vnode)
	{
		auto gpad = JoyStick::get_instance().get_gamepad(_controller);
		VFVec2	pos;
		_vnode->get_position(pos);

		float mag = param.elapsed * .5f;

		pos[0] += gpad.thumb_lx * mag;
		pos[1] += gpad.thumb_ly * mag;

		_vnode->set_poisition(pos);
	}
}

