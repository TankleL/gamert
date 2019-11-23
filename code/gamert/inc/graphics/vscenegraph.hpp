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
	void	update(const VNode::render_param_t& param);
	VNode*	switch_root_node(VNode* new_node);
	void	create_drawcalls(VKRenderer* renderer);
	void	destroy_drawcalls();
	void	set_renderer(VKRenderer* renderer);
	VNode*	get_root_node() const;

private:
	VNode*		_root_node;
	VKRenderer* _renderer;

private:
	static VNode _dummy_node;
};

