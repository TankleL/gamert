#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"
#include "vnode.hpp"

class VKRenderer;
class VSceneGraph
{
public:
	VSceneGraph();
	virtual ~VSceneGraph();

public:
	void update(const VNode::render_param_t& param);
	VNode* switch_root_node(VNode* new_node);

private:
	VNode*		_root_node;

private:
	static VNode _dummy_node;
};

