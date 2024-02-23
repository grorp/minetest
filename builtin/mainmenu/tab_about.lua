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
	"Desour/DS",
	"srifqi",
	"Gregor Parzefall (grorp)",
}

-- currently only https://github.com/orgs/minetest/teams/triagers/members

local core_team = {
	"Zughy [Issue triager]",
	"wsor [Issue triager]",
	"Hugo Locurcio (Calinou) [Issue triager]",
}

-- For updating active/previous contributors, see the script in ./util/gather_git_credits.py

local active_contributors = {
	"Wuzzy [Features, translations, documentation]",
	"numzero [Optimizations, work on OpenGL driver]",
	"ROllerozxa [Bugfixes, Mainmenu]",
	"Lars Müller [Bugfixes]",
	"AFCMS [Documentation]",
	"savilli [Bugfixes]",
	"fluxionary [Bugfixes]",
	"Bradley Pierce (Thresher) [Documentation]",
	"Stvk imension [Android]",
	"JosiahWI [Code cleanups]",
	"OgelGames [UI, Bugfixes]",
	"ndren [Bugfixes]",
	"Abdou-31 [Documentation]",
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
	"Hugues Ross <hugues.ross@gmail.com>",
	"Dmitry Kostenko (x2048) <codeforsmile@gmail.com>",
}

local previous_contributors = {
	"Nils Dagsson Moskopp (erlehmann) <nils@dieweltistgarnichtso.net> [Minetest logo]",
	"red-001 <red-001@outlook.ie>",
	"Giuseppe Bilotta",
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
	local string = table.concat(source, "\n") .. "\n"

	local hypertext_escapes = {
		["\\"] = "\\\\",
		["<"] = "\\<",
		[">"] = "\\>",
	}
	string = string:gsub("[\\<>]", hypertext_escapes)
	string = string:gsub("%[.-%]", "<gray>%1</gray>")

	table.insert(dest, string)
end

local function read_text_file(path)
	local f = io.open(path, "r")
	if not f then
		return "[not available]\n"
	end
    local text = f:read("*all")
    f:close()
	return text
end

local function collect_debug_info()
	local build_info = core.get_build_info()
	return build_info
end

return {
	name = "about",
	caption = fgettext("About"),

	cbf_formspec = function(tabview, name, tabdata)
		local hypertext = {
			"<tag name=heading color=#ff0>",
			"<tag name=gray color=#aaa>",
		}

		table.insert_all(hypertext, {
			"<heading>", fgettext_ne("Core Developers"), "</heading>\n",
		})
		prepare_credits(hypertext, core_developers)
		table.insert_all(hypertext, {
			"\n",
			"<heading>", fgettext_ne("Core Team"), "</heading>\n",
		})
		prepare_credits(hypertext, core_team)
		table.insert_all(hypertext, {
			"\n",
			"<heading>", fgettext_ne("Active Contributors"), "</heading>\n",
		})
		prepare_credits(hypertext, active_contributors)
		table.insert_all(hypertext, {
			"\n",
			"<heading>", fgettext_ne("Previous Core Developers"), "</heading>\n",
		})
		prepare_credits(hypertext, previous_core_developers)
		table.insert_all(hypertext, {
			"\n",
			"<heading>", fgettext_ne("Previous Contributors"), "</heading>\n",
		})
		prepare_credits(hypertext, previous_contributors)

		hypertext = table.concat(hypertext):sub(1, -2)

		local fs = {
			-- This is the right half of the tab (the credits).
			"hypertext[5.5,0.25;9.75,6.6;credits;" .. minetest.formspec_escape(hypertext) .. "]",
		}

		-- Place the content of the left half from bottom to top.
		local TAB_H = 7.1
		local TAB_PADDING = 0.5
		local LOGO_SIZE = 2.5
		local BTN_H = 0.8
		local LABEL_BTN_H = 0.5

		local pos_y = TAB_H - TAB_PADDING
		local show_userdata_btn = PLATFORM ~= "Android"

		if show_userdata_btn then
			pos_y = pos_y - BTN_H
			fs[#fs + 1] = "tooltip[userdata;" ..
					fgettext("Opens the directory that contains user-provided worlds, games, mods,\n" ..
							"and texture packs in a file manager / explorer.") .. "]"
			fs[#fs + 1] = ("button[0.5,%f;4.5,%f;userdata;%s]"):format(
					pos_y, BTN_H, fgettext("Open user data directory"))
			pos_y = pos_y - 0.1
		end

		pos_y = pos_y - BTN_H
		local export_debug_label = PLATFORM == "Android" and fgettext("Share build info") or
				fgettext("Copy build info")
		fs[#fs + 1] = ("button[0.5,%f;4.5,%f;export_debug;%s]"):format(pos_y, BTN_H, export_debug_label)
		pos_y = pos_y - (show_userdata_btn and 0.25 or 0.15)

		pos_y = pos_y - BTN_H
		fs[#fs + 1] = ("button[0.5,%f;4.5,%f;homepage;minetest.net]"):format(pos_y, BTN_H)
		pos_y = pos_y - 0.15

		pos_y = pos_y - LABEL_BTN_H
		local version = core.get_version()
		fs[#fs + 1] = "style[label_button;border=false]"
		fs[#fs + 1] = ("button[0.1,%f;5.3,%f;label_button;%s]"):format(
				pos_y, LABEL_BTN_H, core.formspec_escape(version.project .. " " .. version.string))

		local logofile = core.formspec_escape(defaulttexturedir .. "logo.png")
		-- Place the logo in the middle of the remaining vertical space.
		fs[#fs + 1] = ("image[1.5,%f;%f,%f;%s]"):format(
				pos_y / 2 - LOGO_SIZE / 2, LOGO_SIZE, LOGO_SIZE, logofile)

		return table.concat(fs, "")
	end,

	cbf_button_handler = function(this, fields, name, tabdata)
		if fields.homepage then
			core.open_url("https://www.minetest.net")
		end

		if fields.export_debug then
			local export_fn = PLATFORM == "Android" and core.share_text or core.copy_text
			export_fn(collect_debug_info())
		end

		if fields.userdata then
			core.open_dir(core.get_user_path())
		end
	end,

	on_change = function(type)
		if type == "ENTER" then
			mm_game_theme.set_engine()
		end
	end,
}
