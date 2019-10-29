#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "vulkan/vulkan.hpp"

class VKApplication : public Singleton<VKApplication>
{
	DECL_SINGLETON_CTOR(VKApplication);

public:
	void init(
		const std::string& app_name,
		const std::string& engine_name,
		uint32_t app_version,
		uint32_t engine_version);

private:
	VkInstance					_vkinstance;	// vulkan instance
	VkDebugUtilsMessengerEXT	_vkdbgmsgr;		// vulkan debug messenger
};

