#include "vscenegraph.hpp"
#include "vkrenderer.hpp"

using namespace std;

VNode VSceneGraph::_dummy_node;

VSceneGraph::VSceneGraph()
	: _root_node(&_dummy_node)
	, _renderer(nullptr)
{
	_dummy_node.set_name("dummy");
}

VSceneGraph::~VSceneGraph()
{
	if (_renderer)
	{
		_renderer->unbind_scene_graph();
	}

	assert(_root_node != nullptr);
	if (_root_node != &_dummy_node)
	{
		delete _root_node;
	}
}

void VSceneGraph::update(const VNode::render_param_t& param)
{
	_root_node->render(param);
}

VNode* VSceneGraph::switch_root_node(VNode* new_node)
{
	if (nullptr == new_node)
		new_node = &_dummy_node;

	VNode* old_node = _root_node;
	old_node->uninit();
	old_node->destroy_drawcall();

	new_node->init(this);
	_root_node = new_node;

	if (_renderer)
	{
		_root_node->create_drawcall(_renderer);
	}
	return old_node;
}

void VSceneGraph::create_drawcalls(VKRenderer* renderer)
{
	_root_node->create_drawcall(renderer);
}

void VSceneGraph::destroy_drawcalls()
{
	_root_node->destroy_drawcall();
}

void VSceneGraph::set_renderer(VKRenderer* renderer)
{
	if (renderer && _renderer != renderer)
	{ // set a new renderer to replace the existing one
		_root_node->destroy_drawcall();
		_renderer = renderer;
		_root_node->create_drawcall(_renderer);
	}
	else if (nullptr == renderer && _renderer)
	{ // clean up renderer infomation
		_root_node->destroy_drawcall();
		_renderer = nullptr;
	}
}

VNode* VSceneGraph::get_root_node() const
{
	return _root_node;
}
