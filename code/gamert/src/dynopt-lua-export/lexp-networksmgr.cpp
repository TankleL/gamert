#include "lexp-networksmgr.hpp"
#include "networksmgr.hpp"

EXPORT_LUABINDING(lexp::LExp_NetworksMgr, LExp_NetworksMgr);

void lexp::LExp_NetworksMgr::on_register(lua_State* L)
{
	lua_createtable(L, 0, 2);
	lua_pushcfunction(L, LExp_NetworksMgr::_luafunc_startup);
	lua_setfield(L, -2, "startup");

	lua_setglobal(L, "networksmgr");
}

int lexp::LExp_NetworksMgr::_luafunc_startup(lua_State* L)
{
	NetworksMgr::get_instance().startup();
	return 0;
}


