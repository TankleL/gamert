#include "lexp.hpp"

lexp::LuaExportRegistry::LuaExportRegistry()
{}

void lexp::LuaExportRegistry::register_entries(lua_State* L)
{
	for (const auto& entry : _entries)
	{
		entry->on_register(L);
	}
}

void lexp::LuaExportRegistry::add(std::shared_ptr<ILuaExport> entry)
{
	_entries.push_back(entry);
}

