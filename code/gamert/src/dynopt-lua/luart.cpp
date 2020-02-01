#include "luart.hpp"

lua_State* luart::global_state = nullptr;
std::unordered_set<std::string> luart::_internal::loaded_files;

void luart::init_runtime()
{
	global_state = luaL_newstate();
	luaL_openlibs(global_state);
}

void luart::uninit_runtime()
{
	lua_close(global_state);
}

void luart::run_script(const std::string& filename)
{
	luaL_dofile(global_state, filename.c_str());
	luart::_internal::loaded_files.insert(filename);
}

