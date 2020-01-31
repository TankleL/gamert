#pragma once

#include "vnode.hpp"

class IOptBindVNode
{
public:
	virtual void bind_vnode(VNode* vnode) = 0;
};

