#include "rendermgr.hpp"

VNode RenderMgr::_dummy_vtree;

RenderMgr::RenderMgr()
	: _vtree(&_dummy_vtree)
{}

RenderMgr& RenderMgr::get_instance()
{
	static RenderMgr rmgr;
	return rmgr;
}

void RenderMgr::render_frame()
{
	_vtree->render();
}

