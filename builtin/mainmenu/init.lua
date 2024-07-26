--Minetest
--Copyright (C) 2014 sapier
--
--This program is free software; you can redistribute it and/or modify
--it under the terms of the GNU Lesser General Public License as published by
--the Free Software Foundation; either version 2.1 of the License, or
--(at your option) any later version.
--
--This program is distributed in the hope that it will be useful,
--but WITHOUT ANY WARRANTY; without even the implied warranty of
--MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--GNU Lesser General Public License for more details.
--
--You should have received a copy of the GNU Lesser General Public License along
--with this program; if not, write to the Free Software Foundation, Inc.,
--51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

mt_color_grey  = "#AAAAAA"
mt_color_blue  = "#6389FF"
mt_color_lightblue  = "#99CCFF"
mt_color_green = "#72FF63"
mt_color_dark_green = "#25C191"
mt_color_orange  = "#FF8800"
mt_color_red = "#FF3300"

local menupath = core.get_mainmenu_path()
local basepath = core.get_builtin_path()
defaulttexturedir = core.get_texturepath_share() .. DIR_DELIM .. "base" ..
					DIR_DELIM .. "pack" .. DIR_DELIM

dofile(basepath .. "common" .. DIR_DELIM .. "filterlist.lua")
dofile(basepath .. "fstk" .. DIR_DELIM .. "buttonbar.lua")
dofile(basepath .. "fstk" .. DIR_DELIM .. "dialog.lua")
dofile(basepath .. "fstk" .. DIR_DELIM .. "tabview.lua")
dofile(basepath .. "fstk" .. DIR_DELIM .. "ui.lua")
dofile(basepath .. "fstk" .. DIR_DELIM .. "flow_dialog.lua")
dofile(menupath .. DIR_DELIM .. "async_event.lua")
dofile(menupath .. DIR_DELIM .. "common.lua")
dofile(menupath .. DIR_DELIM .. "serverlistmgr.lua")
dofile(menupath .. DIR_DELIM .. "game_theme.lua")
dofile(menupath .. DIR_DELIM .. "content" .. DIR_DELIM .. "init.lua")

dofile(menupath .. DIR_DELIM .. "dlg_config_world.lua")
dofile(menupath .. DIR_DELIM .. "settings" .. DIR_DELIM .. "init.lua")
dofile(menupath .. DIR_DELIM .. "dlg_create_world.lua")
dofile(menupath .. DIR_DELIM .. "dlg_delete_content.lua")
dofile(menupath .. DIR_DELIM .. "dlg_delete_world.lua")
dofile(menupath .. DIR_DELIM .. "dlg_register.lua")
dofile(menupath .. DIR_DELIM .. "dlg_rename_modpack.lua")
dofile(menupath .. DIR_DELIM .. "dlg_version_info.lua")
dofile(menupath .. DIR_DELIM .. "dlg_reinstall_mtg.lua")

local formspec_ast_path = menupath .. DIR_DELIM .. "formspec_ast"
local flow_path = menupath .. DIR_DELIM .. "flow"

local old_get_modpath = core.get_modpath
core.get_modpath = function(name)
	if name == "formspec_ast" then
		return formspec_ast_path
	end
	if name == "flow" then
		return flow_path
	end
	assert(name == nil)
	old_get_modpath()
end

-- whatever, if it makes them happy
core.get_current_modname = function() return "formspec_ast" end

core.register_on_player_receive_fields = function() end
core.register_on_leaveplayer = function() end
-- we don't want the example formspec chatcommand
core.is_singleplayer = function() return false end
core.get_player_information = function() return {} end

dofile(formspec_ast_path .. DIR_DELIM .. "init.lua")
dofile(flow_path .. DIR_DELIM .. "init.lua")

local tabs = {
	content  = dofile(menupath .. DIR_DELIM .. "tab_content.lua"),
	about = dofile(menupath .. DIR_DELIM .. "tab_about.lua"),
	local_game = dofile(menupath .. DIR_DELIM .. "tab_local.lua"),
	play_online = dofile(menupath .. DIR_DELIM .. "tab_online.lua")
}

--------------------------------------------------------------------------------
local function main_event_handler(tabview, event)
	if event == "MenuQuit" then
		core.close()
	end
	return true
end

--------------------------------------------------------------------------------
local function init_globals()
	-- Init gamedata
	gamedata.worldindex = 0

	menudata.worldlist = filterlist.create(
		core.get_worlds,
		compare_worlds,
		-- Unique id comparison function
		function(element, uid)
			return element.name == uid
		end,
		-- Filter function
		function(element, gameid)
			return element.gameid == gameid
		end
	)

	menudata.worldlist:add_sort_mechanism("alphabetic", sort_worlds_alphabetic)
	menudata.worldlist:set_sortmode("alphabetic")

	mm_game_theme.init()
	mm_game_theme.set_engine() -- This is just a fallback.

	-- Create main tabview
	local tv_main = tabview_create("maintab", {x = 15.5, y = 7.1}, {x = 0, y = 0})

	tv_main:set_autosave_tab(true)
	tv_main:add(tabs.local_game)
	tv_main:add(tabs.play_online)
	tv_main:add(tabs.content)
	tv_main:add(tabs.about)

	tv_main:set_global_event_handler(main_event_handler)
	tv_main:set_fixed_size(false)

	local last_tab = core.settings:get("maintab_LAST")
	if last_tab and tv_main.current_tab ~= last_tab then
		tv_main:set_tab(last_tab)
	end

	tv_main:set_end_button({
		icon = defaulttexturedir .. "settings_btn.png",
		label = fgettext("Settings"),
		name = "open_settings",
		on_click = function(tabview)
			local dlg = create_settings_dlg()
			dlg:set_parent(tabview)
			tabview:hide()
			dlg:show()
			return true
		end,
	})

	ui.set_default("maintab")
	tv_main:show()
	ui.update()

	check_reinstall_mtg()
	check_new_version()
end

local home_gui = dofile(menupath .. "/flow_home.lua")

local dlg = flow_dialog_create("first_test", home_gui, {})
dlg:set_global_event_handler(main_event_handler)

ui.set_default("first_test")
dlg:show()
ui.update()
