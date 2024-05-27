local function show_fullscreen_fs(name)
	local window = minetest.get_player_window_information(name)
	if not window then
		return false, "Unable to get window info"
	end

	print(dump(window))

	local full_size = {
		x = window.max_formspec_size.x * 1.1,
		y = window.max_formspec_size.y * 1.1,
	}

	local factor_x = full_size.x / window.size.x
	local factor_y = full_size.y / window.size.y
	local insets = {
		left   = window.insets.left   * factor_x,
		right  = window.insets.right  * factor_x,
		top    = window.insets.top    * factor_y,
		bottom = window.insets.bottom * factor_y,
	}
	local size = {
		x = full_size.x - insets.left - insets.right,
		y = full_size.y - insets.top  - insets.bottom,
	}
	local have_insets = size.x ~= full_size.x or size.y ~= full_size.y

	local touch_text = window.touch_controls and "Touch controls enabled" or
			"Touch controls disabled"
	local fs = {
		"formspec_version[4]",
		("size[%f,%f]"):format(full_size.x, full_size.y),
		-- With padding ~= 0, the "pixels -> formspec coordinates" conversion
		-- of insets would be much more complicated.
		"padding[0,0]",
		"bgcolor[;true]",
		have_insets and ("box[%f,%f;%f,%f;#f808]"):format(
				insets.left, insets.top, size.x, size.y) or "",
		("container[%f,%f]"):format(insets.left, insets.top),

		("button[%f,%f;1,1;%s;%s]"):format(0, 0, "tl", "TL"),
		("button[%f,%f;1,1;%s;%s]"):format(size.x - 1, 0, "tr", "TR"),
		("button[%f,%f;1,1;%s;%s]"):format(size.x - 1, size.y - 1, "br", "BR"),
		("button[%f,%f;1,1;%s;%s]"):format(0, size.y - 1, "bl", "BL"),

		("label[%f,%f;%s]"):format(size.x / 2, size.y / 2, "Fullscreen"),
		("label[%f,%f;%s]"):format(size.x / 2, size.y / 2 + 1, touch_text),

		"container_end[]",
	}

	minetest.show_formspec(name, "testfullscreenfs:fs", table.concat(fs))
	return true, ("Calculated size of %f, %f"):format(size.x, size.y)
end


minetest.register_chatcommand("testfullscreenfs", {
	func = show_fullscreen_fs,
})
