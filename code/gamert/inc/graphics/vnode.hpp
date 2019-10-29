#pragma once

#include "pre-req.hpp"
#include "vulkan/vulkan.hpp"

class RenderMgr;

/**
 * @desc visual node
 */
class VNode
{
	friend RenderMgr;
public:
	VNode();
	virtual ~VNode();

public:
	virtual void on_init() {};
	virtual void on_prerender() {};
	virtual void on_render(std::vector<VkCommandBuffer>& cmdqueue, VkFramebuffer frmbuffer, VkCommandPool cmdpool) {};
	virtual void on_postrender() {};
	virtual void on_uninit() {};

public:
	void render(std::vector<VkCommandBuffer>& cmdqueue, VkFramebuffer frmbuffer, VkCommandPool cmdpool);

	void set_name(const std::string& name);
	std::string get_name() const;

	void	attach_child(VNode* child);
	VNode*	detach_child(const std::string& name);

private:
	std::vector<VNode*>		_children;
	std::string				_name;
	bool					_visible;
};

