#include "luart.hpp"
#include "resmgr-static.hpp"
#include "lexp.hpp"

lua_State* luart::global_state = nullptr;
std::unordered_set<std::string> luart::_internal::loaded_files;

void luart::init_runtime()
{
	global_state = luaL_newstate();
	luaL_openlibs(global_state);

	luart::_internal::config_lua_package();
	luart::_internal::boot();
	lexp::LuaExportRegistry::get_instance().register_entries(global_state);
}

void luart::uninit_runtime()
{
	lua_close(global_state);
}

void luart::run_script(const std::string& filename)
{
	try
	{
		int status = luaL_dofile(
			global_state,
			ResMgrStatic::get_instance()
				.fullpath(filename)
					.c_str());
		luart::_internal::loaded_files.insert(filename);
		if (status)
		{
			throw status;
		}
	}
	catch (std::exception ex)
	{
		// TODO: handle exceptions
		(void)ex;
		assert(false);
	}
	catch (int status)
	{
		// TODO: handle lua errors
		(void)status;
		const char* msg = lua_tostring(global_state, -1);
		throw std::runtime_error(msg);
		lua_pop(global_state, 1);
	}
}

void luart::_internal::config_lua_package()
{
	// get "package"
	lua_getglobal(global_state, "package");

#if defined(WIN32)
	static const char* path_suffix = "\\scripts\\?.lua";
#else
	static const char* path_suffix = "/scripts/?.lua";
#endif

	std::string package_path =
		ResMgrStatic::get_instance()
			.get_resources_root_path() + path_suffix;

	lua_pushstring(
		global_state,
		package_path.c_str());

	lua_setfield(global_state, -2, "path");

	// pop "package"
	lua_pop(global_state, -1);
}

void luart::_internal::boot()
{
#define _LUART_REQUIRE(mod) "require(\"boot/" mod "\")\n"

	const std::string boot_list =
		_LUART_REQUIRE("class");
	luaL_dostring(global_state, boot_list.c_str());

	int c = lua_gettop(global_state);
}

