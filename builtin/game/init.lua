
local scriptpath = core.get_builtin_path()
local commonpath = scriptpath .. "common" .. DIR_DELIM
local gamepath   = scriptpath .. "game".. DIR_DELIM
local uipath     = scriptpath .. "ui" .. DIR_DELIM

-- Shared between builtin files, but
-- not exposed to outer context
local builtin_shared = {}

dofile(gamepath .. "constants.lua")
assert(loadfile(commonpath .. "item_s.lua"))(builtin_shared)
assert(loadfile(gamepath .. "item.lua"))(builtin_shared)
assert(loadfile(commonpath .. "register.lua"))(builtin_shared)
assert(loadfile(gamepath .. "register.lua"))(builtin_shared)

if core.settings:get_bool("profiler.load") then
	profiler = dofile(scriptpath .. "profiler" .. DIR_DELIM .. "init.lua")
end

dofile(commonpath .. "after.lua")
dofile(commonpath .. "mod_storage.lua")
dofile(gamepath .. "item_entity.lua")
dofile(gamepath .. "deprecated.lua")
dofile(gamepath .. "misc_s.lua")
dofile(gamepath .. "misc.lua")
dofile(gamepath .. "privileges.lua")
dofile(gamepath .. "auth.lua")
dofile(commonpath .. "chatcommands.lua")
dofile(gamepath .. "chat.lua")
dofile(commonpath .. "information_formspecs.lua")
dofile(gamepath .. "static_spawn.lua")
dofile(gamepath .. "detached_inventory.lua")
assert(loadfile(gamepath .. "falling.lua"))(builtin_shared)
dofile(gamepath .. "features.lua")
dofile(gamepath .. "voxelarea.lua")
dofile(gamepath .. "forceloading.lua")
dofile(gamepath .. "statbars.lua")
dofile(gamepath .. "knockback.lua")
dofile(gamepath .. "async.lua")
dofile(uipath .. "init.lua")

core.after(0, builtin_shared.cache_content_ids)

profiler = nil

ui.set_default_theme(ui.Style{
	sel = "root",

	rel_pos = {0.5, 0.5},
	rel_anchor = {0.5, 0.5},
})


local function gui_builder(id, player, cx, param)
	return ui.Window{
		type = "gui",
		root = ui.Root{
			bg_fill = "#fff",
			fg_image = "default_obsidian.png",
			fg_scale = 0,

			ui.Elem {
				rel_size = {0,0},
				size = {200, 200},
				rel_pos = {0.5, 1},
				rel_anchor = {0.5, 1},
				padding = {12, 12, 12, 12},
				-- margin = {24, 24, 24, 24},

				bg_image = "default_dirt.png",
				bg_tint = "#ffffff",

				fg_image = "blank.png",
				fg_scale = 0,
			},
			ui.Elem {

			},
		},
	}
end

local function news_popup(id, player, cx, param)
	local H = 100
	local W = 300
	local SPACE = 10
	local INNER_W = W - 4*SPACE
	local INNER_H = H - 4*SPACE
	return ui.Window{
		type = "message",
		root = ui.Root{
			size = {W, H},
			rel_pos = {1, 1},
			rel_anchor = {1, 1},
			margin = {SPACE, SPACE, SPACE, SPACE},
			padding = {SPACE, SPACE, SPACE, SPACE},

			rel_size = {0, 0},
			bg_fill = "#fff",
			-- not shown, but shown if I remove children.
			-- bug?
			fg_image = "default_sand.png",
			fg_scale = 0,

			ui.Elem {
				fg_image = "default_obsidian.png",
				fg_scale = 0,
				rel_size = {0, 0},
				size = {INNER_H, INNER_H},
			},
			ui.Elem {
				bg_image = "default_dirt.png",
				rel_size = {0, 0},
				margin = {INNER_H + SPACE, 0, 0, 0},
				size = {INNER_W, INNER_H},
			}
		},
	}
end

minetest.register_on_joinplayer(function(player)
	local id = ui.open(news_popup, player:get_player_name())
end)
