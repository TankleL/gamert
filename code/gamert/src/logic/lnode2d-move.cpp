#include "lnode2d-move.hpp"
#include "joystick.hpp"

LNode2dMove::LNode2dMove(int controller)
	: _vnode(nullptr)
	, _controller(controller)
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

void LNode2dMove::on_tick()
{
	if (_vnode)
	{
		auto gpad = JoyStick::get_instance().get_gamepad(_controller);
		VFVec2	pos;
		_vnode->get_position(pos);

		pos[0] += gpad.thumb_lx * 14.4f;
		pos[1] += gpad.thumb_ly * 14.4f;

		_vnode->set_poisition(pos);
	}
}

