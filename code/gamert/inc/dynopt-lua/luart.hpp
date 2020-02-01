#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "lua.hpp"

// lua run-time
namespace luart
{

	/* ************************************************************************
	 * functions
	 * ***********************************************************************/

	void init_runtime();
	void uninit_runtime();
	void run_script(const std::string& filename);

	/* ************************************************************************
	 * variables
	 * ***********************************************************************/

	extern lua_State*		global_state;


	namespace _internal
	{
		extern std::unordered_set<std::string>	loaded_files;
	}
}

