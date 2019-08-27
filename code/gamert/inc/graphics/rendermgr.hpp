#pragma once

#include "pre-req.hpp"
#include "vnode.hpp"

class RenderMgr
{
private:
	RenderMgr();

public:
	static RenderMgr& get_instance();

public:
	void	render_frame();

private:
	static VNode _dummy_vtree;

private:
	VNode*		_vtree;
};
