// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2025 grorp

#include "l_client_common.h"

#include "client/client.h"
#include "client/clientevent.h"
#include "cpp_api/s_base.h"
#include "lua_api/l_internal.h"

// show_formspec(formspec)
int ModApiClientCommon::l_show_formspec(lua_State *L)
{
	if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
		return 0;

	ClientEvent *event = new ClientEvent();
	event->type = getScriptApiBase(L)->getType() == ScriptingType::PauseMenu
			? CE_SHOW_PAUSE_MENU_FORMSPEC
			: CE_SHOW_CSM_FORMSPEC;
	event->show_formspec.formname = new std::string(luaL_checkstring(L, 1));
	event->show_formspec.formspec = new std::string(luaL_checkstring(L, 2));
	getClient(L)->pushToEventQueue(event);
	lua_pushboolean(L, true);
	return 1;
}

void ModApiClientCommon::Initialize(lua_State *L, int top)
{
	API_FCT(show_formspec);
}
