#include "lexp.hpp"

lexp::LuaExportRegistry::LuaExportRegistry()
{}

void lexp::LuaExportRegistry::register_entries(lua_State* L)
{
	_register_lua_gamert_object(L);

	for (const auto& entry : _entries)
	{
		entry->on_register(L);
	}
}

void lexp::LuaExportRegistry::add(std::shared_ptr<ILuaExport> entry)
{
	_entries.push_back(entry);
}

void lexp::LuaExportRegistry::_register_lua_gamert_object(lua_State* L)
{
	lua_createtable(L, 0, 0);
	lua_setglobal(L, "gamert");
}



