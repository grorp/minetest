
core.register_on_esc(function()
	local fs = {[[
		formspec_version[4]
		size[6,10]
		no_prepend[]
		bgcolor[#320000b4;true]
	]]}

	local btns = {
		{ "btn_continue", "Continue" },
		{ "btn_change_password", "Change Password" }
	}

	if PLATFORM ~= "Android" then
		table.insert(btns, { "btn_key_config", "Change Keys" })
	end

	table.insert(btns, { "btn_settings", "Settings" })

	table.insert(btns, { "btn_exit_menu", "Exit to Menu" })

	for i = 1, #btns, 1 do
		table.insert(fs, ("button[1,%s;4,1;%s;%s]"):format(i*1.5, btns[i][1], btns[i][2]))
	end

	core.show_formspec("builtin:esc", table.concat(fs))
end)

core.register_on_formspec_input(function(formname, fields)
	if formname ~= "builtin:esc" then return end

	if fields.btn_continue then
		core.close_formspec()
	end

	if fields.btn_change_password then
		core.close_formspec()
		core.change_password()
	end

	if fields.btn_key_config then
		core.close_formspec()
		core.show_keys_menu()
	end

	if fields.btn_settings then
		create_settings_dlg()
	end

	if fields.btn_exit_menu then
		core.disconnect()
	end
end)
