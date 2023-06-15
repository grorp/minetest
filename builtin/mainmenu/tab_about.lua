--Minetest
--Copyright (C) 2013 sapier
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

-- https://github.com/orgs/minetest/teams/engine/members

local core_developers = {
	"Perttu Ahola (celeron55) <celeron55@gmail.com> [Project founder]",
	"sfan5 <sfan5@live.de>",
	"ShadowNinja <shadowninja@minetest.net>",
	"Nathanaëlle Courant (Nore/Ekdohibs) <nore@mesecons.net>",
	"Loic Blot (nerzhul/nrz) <loic.blot@unix-experience.fr>",
	"Andrew Ward (rubenwardy) <rw@rubenwardy.com>",
	"Krock/SmallJoker <mk939@ymail.com>",
	"Lars Hofhansl <larsh@apache.org>",
	"v-rob <robinsonvincent89@gmail.com>",
	"Hugues Ross <hugues.ross@gmail.com>",
	"Dmitry Kostenko (x2048) <codeforsmile@gmail.com>",
	"Desour",
}

-- currently only https://github.com/orgs/minetest/teams/triagers/members

local core_team = {
	"Zughy [Issue triager]",
	"wsor [Issue triager]",
	"Hugo Locurcio (Calinou) [Issue triager]",
}

-- For updating active/previous contributors, see the script in ./util/gather_git_credits.py

local active_contributors = {
	"Wuzzy [Features, translations, devtest]",
	"Lars Müller [Bugfixes and entity features]",
	"paradust7 [Bugfixes]",
	"ROllerozxa [Bugfixes, Android]",
	"srifqi [Android, translations]",
	"Lexi Hale [Particlespawner animation]",
	"savilli [Bugfixes]",
	"fluxionary [Bugfixes]",
	"Gregor Parzefall [Bugfixes]",
	"Abdou-31 [Documentation]",
	"pecksin [Bouncy physics]",
	"Daroc Alden [Fixes]",
}

local previous_core_developers = {
	"BlockMen",
	"Maciej Kasatkin (RealBadAngel) [RIP]",
	"Lisa Milne (darkrose) <lisa@ltmnet.com>",
	"proller",
	"Ilya Zhuravlev (xyz) <xyz@minetest.net>",
	"PilzAdam <pilzadam@minetest.net>",
	"est31 <MTest31@outlook.com>",
	"kahrl <kahrl@gmx.net>",
	"Ryan Kwolek (kwolekr) <kwolekr@minetest.net>",
	"sapier",
	"Zeno",
	"Auke Kok (sofar) <sofar@foo-projects.org>",
	"Aaron Suen <warr1024@gmail.com>",
	"paramat",
	"Pierre-Yves Rollo <dev@pyrollo.com>",
	"hecks",
	"Jude Melton-Houghton (TurkeyMcMac) [RIP]",
}

local previous_contributors = {
	"Nils Dagsson Moskopp (erlehmann) <nils@dieweltistgarnichtso.net> [Minetest logo]",
	"red-001 <red-001@outlook.ie>",
	"Giuseppe Bilotta",
	"numzero",
	"HybridDog",
	"ClobberXD",
	"Dániel Juhász (juhdanad) <juhdanad@gmail.com>",
	"MirceaKitsune <mirceakitsune@gmail.com>",
	"Jean-Patrick Guerrero (kilbith)",
	"MoNTE48",
	"Constantin Wenger (SpeedProg)",
	"Ciaran Gultnieks (CiaranG)",
	"Paul Ouellette (pauloue)",
	"stujones11",
	"Rogier <rogier777@gmail.com>",
	"Gregory Currie (gregorycu)",
	"JacobF",
	"Jeija <jeija@mesecons.net>",
}

local function prepare_credits(dest, source)
	for _, s in ipairs(source) do
		-- if there's text inside brackets make it gray-ish
		s = s:gsub("%[.-%]", core.colorize("#aaa", "%1"))
		dest[#dest+1] = s
	end
end

local function build_hacky_list(items, spacing)
	spacing = spacing or 0.5
	local y = spacing / 2
	local ret = {}
	for _, item in ipairs(items) do
		if item ~= "" then
			ret[#ret+1] = ("label[0,%f;%s]"):format(y, core.formspec_escape(item))
		end
		y = y + spacing
	end
	return table.concat(ret, ""), y
end

local function read_text_file(path)
	local f = io.open(path, "r")
	if not f then
		return nil
	end
    local text = f:read("*all")
    f:close()
	return text
end

local function get_debug_info()
	local version = core.get_version()
	local path_pre = core.get_user_path() .. DIR_DELIM
	local minetest_conf = read_text_file(path_pre .. "minetest.conf") or "[not available]\n"
	local debug_txt = read_text_file(path_pre .. "debug.txt") or "[not available]\n"

	return table.concat({
		version.project, " ", version.string, "\n",
		"Platform: ", PLATFORM, "\n",
		"Active renderer: ", core.get_active_renderer(), "\n\n",
		"minetest.conf\n",
		"_____________\n",
		minetest_conf,
		"\ndebug.txt\n",
		"_________\n",
		debug_txt,
	})
end

return {
	name = "about",
	caption = fgettext("About"),
	cbf_formspec = function(tabview, name, tabdata)
		local logofile = defaulttexturedir .. "logo.png"
		local version = core.get_version()

		local credit_list = {}
		table.insert_all(credit_list, {
			core.colorize("#000", "Dedication of the current release"),
			"The 5.7.0 release is dedicated to the memory of",
			"Minetest developer Jude Melton-Houghton (TurkeyMcMac)",
			"who died on February 1, 2023.",
			"Our thoughts are with his family and friends.",
			"",
			core.colorize("#ff0", fgettext("Core Developers"))
		})
		prepare_credits(credit_list, core_developers)
		table.insert_all(credit_list, {
			"",
			core.colorize("#ff0", fgettext("Core Team"))
		})
		prepare_credits(credit_list, core_team)
		table.insert_all(credit_list, {
			"",
			core.colorize("#ff0", fgettext("Active Contributors"))
		})
		prepare_credits(credit_list, active_contributors)
		table.insert_all(credit_list, {
			"",
			core.colorize("#ff0", fgettext("Previous Core Developers"))
		})
		prepare_credits(credit_list, previous_core_developers)
		table.insert_all(credit_list, {
			"",
			core.colorize("#ff0", fgettext("Previous Contributors"))
		})
		prepare_credits(credit_list, previous_contributors)
		local credit_fs, scroll_height = build_hacky_list(credit_list)
		-- account for the visible portion
		scroll_height = math.max(0, scroll_height - 6.9)

		local fs = "image[1.5,0.4;2.5,2.5;" .. core.formspec_escape(logofile) .. "]" ..
			"style[label_button;border=false]" ..
			"button[0.1,3.1;5.3,0.5;label_button;" ..
			core.formspec_escape(version.project .. " " .. version.string) .. "]" ..
			"button[0.5,3.85;4.5,0.8;homepage;minetest.net]" ..
			"scroll_container[5.5,0.1;9.5,6.9;scroll_credits;vertical;" ..
			tostring(scroll_height / 1000) .. "]" .. credit_fs ..
			"scroll_container_end[]"..
			"scrollbar[15,0.1;0.4,6.9;vertical;scroll_credits;0]"

		local debug_label = PLATFORM == "Android" and fgettext("Share debug info") or
				fgettext("Copy debug info")
		fs = fs .. "button[0.5,4.9;4.5,0.8;share_debug;" .. debug_label .. "]"

		if PLATFORM ~= "Android" then		
			fs = fs .. "tooltip[userdata;" ..
					fgettext("Opens the directory that contains user-provided worlds, games, mods,\n" ..
							"and texture packs in a file manager / explorer.") .. "]"
			fs = fs .. "button[0.5,5.8;4.5,0.8;userdata;" .. fgettext("Open user data directory") .. "]"
		end

		return fs, "size[15.5,7.1,false]real_coordinates[true]"
	end,
	cbf_button_handler = function(this, fields, name, tabdata)
		if fields.homepage then
			core.open_url("https://www.minetest.net")
		end

		if fields.share_debug then
			local info = get_debug_info()
			if PLATFORM == "Android" then
				core.share_text(info)
			else
				core.copy_text(info)
			end
		end

		if fields.userdata then
			core.open_dir(core.get_user_path())
		end
	end,
}
