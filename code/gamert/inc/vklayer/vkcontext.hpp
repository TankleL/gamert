#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "vulkan/vulkan.hpp"
#include "vkdevice.hpp"
#include "vkswapchain.hpp"
#include "vkrenderer.hpp"

class VKContext : public Singleton<VKContext>
{
	DECL_SINGLETON_CTOR(VKContext);
public:
	void init(
		const std::string& app_name,
		const std::string& engine_name,
		uint32_t app_version,
		uint32_t engine_version,
		void* hwnd,
		int max_frames_in_flight = 2);

	void register_renderer(VKRenderer* renderer);
	void unregister_all_renderers();

	void wait_device_idle();
	void resize();
	void destroy();

public:
	inline VKDevice*	get_device() const { return _device; };
	inline bool			get_flag_resized() const { return _flag_resized; }
	inline void			set_flag_resized(bool value) { _flag_resized = value; }

	GRT_VULKAN_FACTOR_GETTER(VkSurfaceKHR, surface, _vksrf);
	GRT_VULKAN_FACTOR_GETTER(VkDevice, device, _device->get_vulkan_device());
	GRT_VULKAN_FACTOR_GETTER(VkPhysicalDevice, physical_device, _device->get_vulkan_physical_device());
	GRT_VULKAN_FACTOR_GETTER(const std::vector<VkSemaphore>&, semaphores_image_avaliable, _sp_imgavaliable);
	GRT_VULKAN_FACTOR_GETTER(const std::vector<VkSemaphore>&, semaphores_rendering_finished, _sp_rdrfinished);
	GRT_VULKAN_FACTOR_GETTER(const std::vector<VkFence>&, fences_inflight, _fn_inflight);
	GRT_VULKAN_FACTOR_GETTER(int, max_frames_in_flight, _max_frames_in_flight);
	GRT_VULKAN_FACTOR_GETTER(int, current_frame_index, _cur_frame);
	GRT_VULKAN_FACTOR_GETTER(VkCommandPool, command_pool, _vkcmdpool);

public:
	inline void next_frame_index() { _cur_frame = (_cur_frame + 1) % _max_frames_in_flight; }

private:
	std::vector<VkSemaphore>	_sp_imgavaliable;	// vulkan semaphores - image avaliable
	std::vector<VkSemaphore>	_sp_rdrfinished;	// vulkan semaphores - rendering finished
	std::vector<VkFence>		_fn_inflight;		// vulkan in-flight fences
	VkSurfaceKHR				_vksrf;				// vulkan surface
	VkInstance					_vkinstance;		// vulkan instance
	VkDebugUtilsMessengerEXT	_vkdbgmsgr;			// vulkan debug messenger
	VkCommandPool				_vkcmdpool;

private:
	std::vector<VKRenderer*>	_renderers;
	void*						_hwnd;
	VKDevice*					_device;
	VKSwapchain*				_swapchain;
	int							_max_frames_in_flight;
	int							_cur_frame;
	bool						_flag_resized;
};

