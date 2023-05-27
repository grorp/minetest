--Minetest
--Copyright (C) 2022 rubenwardy
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

local make = {}


-- This file defines various component constructors, of the form:
--
--     make.component(setting)
--
-- `setting` is a table representing the settingtype.
--
-- A component is a table with the following:
--
-- * `full_width`: (Optional) true if the component shouldn't reserve space for info / reset.
-- * `info_text`: (Optional) string, informational text shown in an info icon.
-- * `setting`: (Optional) the setting.
-- * `max_w`: (Optional) maximum width, `avail_w` will never exceed this.
-- * `changed`: (Optional) true if the setting has changed from its default value.
-- * `get_formspec = function(self, avail_w)`:
--     * `avail_w` is the available width for the component.
--     * Returns `fs, used_height`.
--     * `fs` is a string for the formspec.
--       Components should be relative to `0,0`, and not exceed `avail_w` or the returned `used_height`.
--     * `used_height` is the space used by components in `fs`.
-- * `on_submit = function(self, fields, parent)`:
--     * `fields`: submitted formspec fields
--     * `parent`: the fstk element for the settings UI, use to show dialogs
--     * Return true if the event was handled, to prevent future components receiving it.


local function get_label(setting)
	local show_technical_names = core.settings:get_bool("show_technical_names")
	if not show_technical_names and setting.readable_name then
		return fgettext(setting.readable_name)
	end
	return setting.name
end


local function is_valid_number(value)
	return type(value) == "number" and not (value ~= value or value >= math.huge or value <= -math.huge)
end


function make.heading(text)
	return {
		full_width = true,
		get_formspec = function(self, avail_w)
			return ("label[0,0.6;%s]box[0,0.9;%f,0.05;#ccc6]"):format(core.formspec_escape(text), avail_w), 1.2
		end,
	}
end


--- Used for string and numeric style fields
---
--- @param converter Function to coerce values
--- @param validator Validator function, optional. Returns true when valid.
local function make_field(converter, validator)
	return function(setting)
		return {
			info_text = setting.comment,
			setting = setting,

			get_formspec = function(self, avail_w)
				local value = core.settings:get(setting.name) or setting.default
				self.changed = converter(value) ~= converter(setting.default)

				local fs = ("field[0,0.3;%f,0.8;%s;%s;%s]"):format(
					avail_w - 2.3, setting.name, get_label(setting), core.formspec_escape(value))
				fs = fs .. ("button[%f,0.3;1.5,0.8;%s;%s]"):format(avail_w - 2, "set_" .. setting.name, fgettext("Set"))

				return fs, 1.1
			end,

			on_submit = function(self, fields)
				if fields["set_" .. setting.name] or fields.key_enter_field == setting.name then
					local value = converter(fields[setting.name])
					if value == nil or (validator and not validator(value)) then
						return true
					end

					if setting.min then
						value = math.max(value, setting.min)
					end
					if setting.max then
						value = math.min(value, setting.max)
					end
					core.settings:set(setting.name, tostring(value))
					return true
				end
			end,
		}
	end
end


make.float = make_field(tonumber, is_valid_number)
make.int = make_field(function(x)
	local value = tonumber(x)
	return value and math.floor(value)
end, is_valid_number)
make.string = make_field(tostring, nil)


function make.bool(setting)
	return {
		info_text = setting.comment,
		setting = setting,

		get_formspec = function(self, avail_w)
			local value = core.settings:get_bool(setting.name, core.is_yes(setting.default))
			self.changed = tostring(value) ~= setting.default

			local fs = ("checkbox[0,0.25;%s;%s;%s]"):format(
				setting.name, get_label(setting), tostring(value))
			return fs, 0.5
		end,

		on_submit = function(self, fields)
			if fields[setting.name] == nil then
				return false
			end

			core.settings:set_bool(setting.name, core.is_yes(fields[setting.name]))
			return true
		end,
	}
end


function make.enum(setting)
	return {
		info_text = setting.comment,
		setting = setting,
		max_w = 4.5,

		get_formspec = function(self, avail_w)
			local value = core.settings:get(setting.name) or setting.default
			self.changed = value ~= setting.default

			local items = {}
			for i, option in ipairs(setting.values) do
				items[i] = core.formspec_escape(option)
			end

			local selected_idx = table.indexof(setting.values, value)
			local fs = "label[0,0.1;" .. get_label(setting) .. "]"

			fs = fs .. ("dropdown[0,0.35;%f,0.8;%s;%s;%d]"):format(
				avail_w, setting.name, table.concat(items, ","), selected_idx, value)

			return fs, 1.1
		end,

		on_submit = function(self, fields)
			local old_value = core.settings:get(setting.name) or setting.default
			local value = fields[setting.name]
			if value == nil or value == old_value then
				return false
			end

			core.settings:set(setting.name, value)
			return true
		end,
	}
end

make.filepath = make.string
make.path = make.string

return make
