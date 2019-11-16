#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"
#include "vdrawcall.hpp"

class VKRenderer;
class VSceneGraph;

/**
 * @desc visual node
 */
class VNode
{
public:
	typedef struct _st_render_param
	{
		VkCommandBuffer		cmd;
		VkPipeline			pipeline;
		VkPipelineLayout	pipeline_layout;
		int					fbo_index;
		float				elapsed;
	} render_param_t;

public:
	VNode();
	virtual ~VNode();

public:
	virtual void on_init() {};
	virtual void on_managed() {};
	virtual void on_prerender() {};
	virtual void on_render(const render_param_t& param) {};
	virtual void on_create_drawcall(VKRenderer* renderer) {}
	virtual void on_destroy_drawcall() {};
	virtual void on_postrender() {};
	virtual void on_detached() {};
	virtual void on_uninit() {};

public:
	void init(VSceneGraph* scene);
	void render(const render_param_t& param);
	void create_drawcall(VKRenderer* renderer);
	void destroy_drawcall();
	void uninit();
	bool is_managed() const;

	void set_name(const std::string& name);
	std::string get_name() const;

	void	manage_child(VNode* child);
	VNode*	detach_child(const std::string& name);
	void	detach_child(VNode* node);
	void	detach();

protected:
	VDrawCall*				_drawcall;

private:
	std::vector<VNode*>		_children;
	std::string				_name;
	VSceneGraph*			_scene;
	VNode*					_parent;
	bool					_visible;
};

