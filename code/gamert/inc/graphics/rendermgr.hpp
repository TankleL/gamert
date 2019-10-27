#pragma once

#include "pre-req.hpp"
#include "vnode.hpp"

#include <vulkan/vulkan.hpp>

class RenderMgr
{
private:
	RenderMgr();

public:
	enum BuiltinPipelines
	{
		BTPL_2D_Position_RGBColor	= 0		// Posistion
	};

public:
	class Pipeline
	{
		friend RenderMgr;
	public:
		Pipeline() noexcept;
		Pipeline(
			uint32_t pipeline_id,
			const std::string& vert_shader_path,
			const std::string& frag_shader_path) noexcept;

		Pipeline(const Pipeline& rhs) noexcept;
		Pipeline(Pipeline&& rhs) noexcept;

		Pipeline& operator=(const Pipeline& rhs) noexcept;
		Pipeline& operator=(Pipeline&& rhs) noexcept;

	public:
		uint32_t	id;
		std::string	shader_vert_path;
		std::string	shader_frag_path;

	private:
		VkPipelineLayout	_pipeline_layout;
		VkPipeline			_pipeline;
	};

public:
	static RenderMgr& get_instance();

public:
	void	set_vulkan_device(VkDevice device);
	void	set_vulkan_physical_device(VkPhysicalDevice phydev);
	void	set_vulkan_renderpass(VkRenderPass renderpass);
	void	set_vulkan_swapchain_extent(VkExtent2D extent);

	VkDevice			get_vulkan_device() const;
	VkPhysicalDevice	get_vulkan_physical_device() const;

public:
	void		render_frame();
	void		create_pipelines();
	void		destroy_pipelines();
	VkPipeline	get_vulkan_pipeline(BuiltinPipelines id);

private:
	void	_create_builtin_pipelines();

private:
	static VNode _dummy_vtree;

private:
	std::unordered_map<uint32_t, Pipeline>	_pipelines;

private:
	VNode*				_vtree;
	VkDevice			_vkdevice;
	VkPhysicalDevice	_vkphydev;
	VkRenderPass		_vkrdrpass;
	VkExtent2D			_vkscext;
};
