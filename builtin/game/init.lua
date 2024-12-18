
local scriptpath = core.get_builtin_path()
local commonpath = scriptpath .. "common" .. DIR_DELIM
local gamepath   = scriptpath .. "game".. DIR_DELIM

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
dofile(commonpath .. "metatable.lua")
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
dofile(gamepath .. "hud.lua")
dofile(gamepath .. "knockback.lua")
dofile(gamepath .. "async.lua")
dofile(gamepath .. "death_screen.lua")

core.after(0, builtin_shared.cache_content_ids)

profiler = nil


local function clean_vec_print(v)
	return string.format("%.3f, %.3f, %.3f", v.x, v.y, v.z)
end

function haha()
	for _, p in ipairs(core.get_connected_players()) do
		p:set_look_horizontal(math.random() * math.pi * 4)
		p:set_look_vertical(math.random() * math.pi - math.pi/2)

		-- Basically the usefulness of storing the point dir relative to the
		-- look dir is this:
		-- > When the server sets the look dir, the server knows the correct new point dir,
		-- > even before the network round-trip to the client
		-- or
		-- > The two values printed below are equal
		-- > (assuming no look dir / point dir changes happen on the client in the meantime)

		-- This works both on desktop (or with touch_use_crosshair = true) and with
		-- touch_use_crosshair = false.
		-- To use this test code with touch_use_crosshair = false, just put your
		-- finger anywhere on the screen.

		core.log("error", "server-side point dir = " .. clean_vec_print(p:get_point_dir()) .. " (before round-trip)")
		core.after(0.5, function()
			core.log("error", "server-side point dir = " .. clean_vec_print(p:get_point_dir()) .. " (after round-trip)")
		end)
	end

	core.after(2, haha)
end

core.after(2, haha)
