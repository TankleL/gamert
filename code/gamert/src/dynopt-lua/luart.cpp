#include "luart.hpp"
#include "resmgr-static.hpp"
#include "lexp.hpp"

lua_State* luart::global_state = nullptr;
std::unordered_set<std::string> luart::_internal::loaded_files;

void luart::init_runtime()
{
	global_state = luaL_newstate();
	luaL_openlibs(global_state);

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

