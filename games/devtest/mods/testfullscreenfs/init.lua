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

	-- We set a negative formspec padding to extend the formspec a little bit
	-- past the edge of the screen, avoiding a visible border. Then, we compensate
	-- for the negative padding manually by applying its inverse.
	-- You can set "padding" to any negative value, the layout won't fall apart.
	local padding = -0.1
	-- For reference, search for "use_imgsize" in "guiFormspecMenu.cpp".
	local padded_screensize = {
		x = window.size.x * (1 - padding * 2),
		y = window.size.y * (1 - padding * 2),
	}
	-- Assumption: Our formspec's aspect ratio matches the window aspect ratio
	-- and we use the same padding on both axes.
	local coord_size = padded_screensize.x / full_size.x
	local inverse_padding = {
		x = padded_screensize.x - window.size.x,
		y = padded_screensize.y - window.size.y,
	}
	local insets = {
		left   = inverse_padding.x / coord_size / 2 + window.insets.left   / coord_size,
		right  = inverse_padding.x / coord_size / 2 + window.insets.right  / coord_size,
		top    = inverse_padding.y / coord_size / 2 + window.insets.top    / coord_size,
		bottom = inverse_padding.y / coord_size / 2 + window.insets.bottom / coord_size,
	}
	local size = {
		x = full_size.x - insets.left - insets.right,
		y = full_size.y - insets.top  - insets.bottom,
	}

	local touch_text = window.touch_controls and "Touch controls enabled" or
			"Touch controls disabled"
	local fs = {
		"formspec_version[4]",
		("size[%f,%f]"):format(full_size.x, full_size.y),
		("padding[%f,%f]"):format(padding, padding),
		("box[%f,%f;%f,%f;#f808]"):format(insets.left, insets.top, size.x, size.y),
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
	return true, ("Calculated size of %f, %f"):format(full_size.x, full_size.y)
end


minetest.register_chatcommand("testfullscreenfs", {
	func = show_fullscreen_fs,
})
