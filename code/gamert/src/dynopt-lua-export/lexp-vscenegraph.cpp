#include "lexp-vscenegraph.hpp"
#include "vscenegraph.hpp"

EXPORT_LUABINDING(lexp::LExp_VSceneGraph, LExp_VSceneGraph);

void lexp::LExp_VSceneGraph::on_register(lua_State* L)
{
	// VSceneGraph - create a lua table
	lua_createtable(L, 0, 2);

	// name it
	lua_setglobal(L, "VSceneGraph");
}



