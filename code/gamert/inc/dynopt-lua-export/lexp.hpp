#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "lua.hpp"


namespace lexp
{
	class ILuaExport
	{
	public:
		virtual void on_register(lua_State* L) = 0;
	};

	class LuaExportRegistry : public Singleton<LuaExportRegistry>
	{
		DECL_SINGLETON_CTOR(LuaExportRegistry);

	public:
		void register_entries(lua_State* L);
	
	public:
		void add(std::shared_ptr<ILuaExport> entry);

	private:
		std::vector<std::shared_ptr<ILuaExport>>	_entries;
	};
	
	template<class _LuaExportType>
	class ExportLuaBinding
	{
	public:
		ExportLuaBinding()
		{
			LuaExportRegistry::get_instance().add(std::make_shared<_LuaExportType>());
		}
	};

#define EXPORT_LUABINDING(typelong, typeshort) \
			lexp::ExportLuaBinding<typelong> _iv_exp_lb_##typeshort
}

