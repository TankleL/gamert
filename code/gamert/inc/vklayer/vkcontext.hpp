#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "vulkan/vulkan.hpp"
#include "vkdevice.hpp"

#define VULKAN_FACTOR_GETTER(factor_type, factor_name, private_var_name)	\
	factor_type get_##factor_name() const	{return private_var_name;}

class VKContext : public Singleton<VKContext>
{
	DECL_SINGLETON_CTOR(VKContext);
public:
	void init(
		const std::string& app_name,
		const std::string& engine_name,
		uint32_t app_version,
		uint32_t engine_version,
		void* hwnd);
	void uninit();

public:
	VULKAN_FACTOR_GETTER(VkSurfaceKHR, vulkan_surface, _vksrf);

private:
	VKDevice*	_device;

private:
	VkSurfaceKHR				_vksrf;			// vulkan surface
	VkInstance					_vkinstance;	// vulkan instance
	VkDebugUtilsMessengerEXT	_vkdbgmsgr;		// vulkan debug messenger
};

#undef VULKAN_FACTOR_GETTER
