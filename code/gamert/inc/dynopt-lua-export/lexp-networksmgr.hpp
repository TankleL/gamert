#pragma once

#include "pre-req.hpp"
#include "lexp.hpp"

namespace lexp
{
	class LExp_NetworksMgr : public ILuaExport
	{
	public:
		virtual void on_register(lua_State* L) override;

	public:
		static int _luafunc_startup(lua_State* L);
		static int _luafunc_shutdown(lua_State* L);
		static int _luafunc_reconnect(lua_State* L);
	};
}
