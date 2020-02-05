#include "lexp-networksmgr.hpp"
#include "networksmgr.hpp"

EXPORT_LUABINDING(lexp::LExp_NetworksMgr, LExp_NetworksMgr);

void lexp::LExp_NetworksMgr::on_register(lua_State* L)
{
	// networksmgr - create a lua object
	lua_createtable(L, 0, 2);

	// add a function - startup()
	lua_pushcfunction(L, LExp_NetworksMgr::_luafunc_startup);
	lua_setfield(L, -2, "startup");

	// add a function - shutdown()
	lua_pushcfunction(L, LExp_NetworksMgr::_luafunc_shutdown);
	lua_setfield(L, -2, "shutdown");
	
	// add a function - reconnect
	lua_pushcfunction(L, LExp_NetworksMgr::_luafunc_reconnect);
	lua_setfield(L, -2, "reconnect");

	// name it
	lua_setglobal(L, "networksmgr");
}

int lexp::LExp_NetworksMgr::_luafunc_startup(lua_State* L)
{
	NetworksMgr::get_instance().startup();
	return 0;
}

int lexp::LExp_NetworksMgr::_luafunc_shutdown(lua_State* L)
{
	NetworksMgr::get_instance().shutdown();
	return 0;
}

int lexp::LExp_NetworksMgr::_luafunc_reconnect(lua_State* L)
{
	int argc = lua_gettop(L);

	bool param_force = false; // default

	if (argc >= 1)
	{
		if (lua_isboolean(L, 1))
		{
			param_force = lua_toboolean(L, 1);
		}
		else
		{
			LUA_REPORT_ERROR(L, "bad arguments.");
		}
	}

	NetworksMgr::get_instance().reconnect(param_force);

	return 0;
}
