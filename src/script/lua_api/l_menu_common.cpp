// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2013 sapier
// Copyright (C) 2025 grorp

#include "l_menu_common.h"

#include "client/renderingengine.h"
#include "gettext.h"
#include "lua_api/l_internal.h"
#include <mutex>
#include "threading/mutex_auto_lock.h"


int ModApiMenuCommon::l_gettext(lua_State *L)
{
	const char *srctext = luaL_checkstring(L, 1);
	const char *text = *srctext ? gettext(srctext) : "";
	lua_pushstring(L, text);

	return 1;
}


int ModApiMenuCommon::l_get_active_driver(lua_State *L)
{
	auto drivertype = RenderingEngine::get_video_driver()->getDriverType();
	lua_pushstring(L, RenderingEngine::getVideoDriverInfo(drivertype).name.c_str());
	return 1;
}


int ModApiMenuCommon::l_irrlicht_device_supports_touch(lua_State *L)
{
	lua_pushboolean(L, RenderingEngine::get_raw_device()->supportsTouchEvents());
	return 1;
}


// this is intentionally a global and not part of MainMenuScripting or such
namespace {
	std::unordered_map<std::string, std::string> once_values;
	std::mutex once_mutex;
}

int ModApiMenuCommon::l_set_once(lua_State *L)
{
	std::string key = readParam<std::string>(L, 1);
	if (lua_isnil(L, 2))
		return 0;
	std::string value = readParam<std::string>(L, 2);

	{
		MutexAutoLock lock(once_mutex);
		once_values[key] = value;
	}

	return 0;
}

int ModApiMenuCommon::l_get_once(lua_State *L)
{
	std::string key = readParam<std::string>(L, 1);

	{
		MutexAutoLock lock(once_mutex);
		auto it = once_values.find(key);
		if (it == once_values.end())
			lua_pushnil(L);
		else
			lua_pushstring(L, it->second.c_str());
	}

	return 1;
}

void ModApiMenuCommon::Initialize(lua_State *L, int top)
{
	API_FCT(gettext);
	API_FCT(get_active_driver);
	API_FCT(irrlicht_device_supports_touch);
	API_FCT(set_once);
	API_FCT(get_once);
}


void ModApiMenuCommon::InitializeAsync(lua_State *L, int top)
{
	API_FCT(gettext);
}
