#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"
#include "vvertex.hpp"

class VKRenderer;

/**
 * @desc visual node
 */
class VNode
{
public:
	typedef struct _st_render_param
	{
		VkCommandBuffer		cmd;
		const VKRenderer*	renderer;
		float				elapsed;
	} render_param_t;

public:
	VNode();
	virtual ~VNode();

public:
	virtual void on_init() {};
	virtual void on_prerender() {};
	virtual void on_render(const render_param_t& param) {};
	virtual void on_postrender() {};
	virtual void on_uninit() {};

public:
	void init();
	void render(const render_param_t& param);
	void uninit();

	void set_name(const std::string& name);
	std::string get_name() const;

	void	attach_child(VNode* child);
	VNode*	detach_child(const std::string& name);

private:
	VFVec3					_position;
	VFVec3					_rotation;
	VFVec3					_scale;
	std::vector<VNode*>		_children;
	std::string				_name;
	bool					_visible;
};

