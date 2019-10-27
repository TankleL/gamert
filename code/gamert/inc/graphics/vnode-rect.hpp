#pragma once

#include "pre-req.hpp"
#include "vnode.hpp"
#include "vvertex.hpp"

class VNodeRect : public VNode
{
public:
	virtual void on_init();

protected:
	VVertex2DRGB	_vertices[3];
};


