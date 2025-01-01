// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2025 grorp

#include "s_pause_menu.h"
#include "cpp_api/s_internal.h"

void ScriptApiPauseMenu::open_settings()
{
	SCRIPTAPI_PRECHECKHEADER

	int error_handler = PUSH_ERROR_HANDLER(L);

	// Get handler function
	lua_getglobal(L, "core");
	lua_getfield(L, -1, "open_settings");
	lua_remove(L, -2); // Remove core
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1); // Pop open_settings
		return;
	}
	luaL_checktype(L, -1, LUA_TFUNCTION);

	// Call it
	PCALL_RES(lua_pcall(L, 0, 0, error_handler));
	lua_pop(L, 1); // Pop error handler
}
