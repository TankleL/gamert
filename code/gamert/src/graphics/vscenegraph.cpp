#include "vscenegraph.hpp"

using namespace std;

VNode VSceneGraph::_dummy_node;

VSceneGraph::VSceneGraph()
	: _root_node(&_dummy_node)
{
	_dummy_node.set_name("dummy");
}

VSceneGraph::~VSceneGraph()
{}

void VSceneGraph::update(VKRenderer* renderer, VkCommandBuffer cmd, int fbo_index, float elapsed)
{
	VNode::render_param_t	param = {};
	param.cmd = cmd;
	param.renderer = renderer;
	param.elapsed = elapsed;
	param.fbo_index = fbo_index;
	_root_node->render(param);
}

VNode* VSceneGraph::switch_root_node(VNode* new_node)
{
	if (nullptr == new_node)
		new_node = &_dummy_node;

	VNode* old_node = _root_node;
	old_node->uninit();
	new_node->init();
	_root_node = new_node;
	return old_node;
}

