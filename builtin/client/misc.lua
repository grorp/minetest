function core.setting_get_pos(name)
	local value = core.settings:get(name)
	if not value then
		return nil
	end
	return core.string_to_pos(value)
end


-- old non-method sound functions

function core.sound_stop(handle, ...)
	return handle:stop(...)
end

function core.sound_fade(handle, ...)
	return handle:fade(...)
end

-- Close any and all formspecs that are currently open clientside
function core.close_formspec()
	return core.show_formspec("", "")
end
