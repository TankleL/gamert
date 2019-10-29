#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "vnode.hpp"

#include <vulkan/vulkan.hpp>

class RenderMgr : public SingletonTemplate<RenderMgr>
{
	DECL_SINGLETON_CTOR(RenderMgr);

public:
	enum BuiltinPipelines : uint32_t
	{
		BTPL_2D_Position_RGBColor	= 0		// Posistion
	};

	enum BuiltinRenderPasses : uint32_t
	{
		BTRP_Default = 0,
		BTRP_CleanUp
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
	void	set_vulkan_device(VkDevice device);
	void	set_vulkan_physical_device(VkPhysicalDevice phydev);
	void	set_vulkan_swapchain_extent(VkExtent2D extent);
	void	set_vulkan_swapchain_image_format(VkFormat fmt);
	void	set_vulkan_command_pool(VkCommandPool pool);

	VkDevice			get_vulkan_device() const;
	VkPhysicalDevice	get_vulkan_physical_device() const;
	VkCommandPool		get_vulkan_command_pool() const;
	VkExtent2D			get_vulkan_swapchain_extent() const;
	VkPipeline			get_vulkan_pipeline(BuiltinPipelines id);
	std::vector<VkCommandBuffer>& get_vulkan_commands();
	void				clear_vulkan_commands();

public:
	void		render_frame(VkFramebuffer frmbuffer);
	void		create_renderpasses();
	void		create_pipelines();
	void		destroy_pipelines();
	void		destroy_renderpasses();
	VNode*		switch_vtree(VNode* tree);

private:
	void	_create_builtin_renderpass();
	void	_create_builtin_pipelines();

private:
	static VNode _dummy_vtree;

private:
	std::unordered_map<uint32_t, Pipeline>		_pipelines;
	std::unordered_map<uint32_t, VkRenderPass>	_renderpasses;
	std::vector<VkCommandBuffer>				_cmdqueue;
	VNode* _vtree;

private:
	VkDevice			_vkdevice;
	VkPhysicalDevice	_vkphydev;
	VkFormat			_vkscimgfmt;
	VkExtent2D			_vkscext;
	VkCommandPool		_vkcmdpool;
};
