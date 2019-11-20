#pragma once

#include "lnode.hpp"
#include "vnode2d.hpp"

class LNode2dMove : public LNode
{
public:
	LNode2dMove(int controller);
	virtual ~LNode2dMove();

public:
	void bind(VNode2d* node);
	void set_controller(int controller);

public:
	virtual void on_tick() override;

private:
	VNode2d*		_vnode;
	int				_controller;
};
