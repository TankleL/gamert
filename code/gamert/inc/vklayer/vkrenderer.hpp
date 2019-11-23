#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"

class VKSwapchain;
class VSceneGraph;
class VKRenderer
{
public:
	virtual void		bind_scene_graph(VSceneGraph* graph) {};
	virtual void		unbind_scene_graph() {};
	virtual void		init(VKSwapchain* swapchain) = 0;
	virtual void		unint() = 0;
	virtual void		update(float elapsed) = 0;
};
