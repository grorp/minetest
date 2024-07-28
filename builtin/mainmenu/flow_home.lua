local gui = flow.widgets
local S = fgettext_ne


local home = flow.make_gui(function(player, ctx)
    local size = core.get_window_info().max_formspec_size
    local ENABLE_TOUCH = core.settings:get_bool("enable_touch")

    local HEADER_I_H = 2
    local HEADER_T_H = 1.5

    local BTN_H = ENABLE_TOUCH and 1.25 or 1

    return gui.VBox {
        bg_fullscreen = "neither",
        min_w = size.x,
        min_h = size.y,

        gui.Spacer{},
        gui.HBox {
            align_h = "center",
            gui.Image { w = HEADER_I_H, h = HEADER_I_H, texture_name = defaulttexturedir .. "/logo.png" },
            gui.Spacer { expand = false, w = 0.1 },
            gui.Image { w = 127 / 16 * HEADER_T_H, h = HEADER_T_H, texture_name = defaulttexturedir .. "/menu_header.png" },
        },
        gui.Spacer{},

        gui.VBox {
            align_h = "centre",
            align_v = "centre",
            -- replaced by gui.Spacer{}s
            -- expand = true,
            min_w = 5,

            gui.Button{ label = S"Singleplayer", h = BTN_H, on_event = function(player, ctx)
                local dlg = flow_dialog_create("flow_gamelist", gamelist_gui, {})
                dlg:set_parent(ctx.fstk_dialog)
                ctx.fstk_dialog:hide()
                dlg:show()
                ui.update()
            end},
            gui.Button{ label = S"Multiplayer", h = BTN_H },
            gui.Button{ label = S"Packages", h = BTN_H, on_event = function(player, ctx)
                local dlg = create_contentdb_dlg()
                dlg:set_parent(ctx.fstk_dialog)
                ctx.fstk_dialog:hide()
                dlg:show()
                ui.update()
            end},

            gui.Spacer { expand = false, h = 0.2 },
            gui.Button{ label = S"Settings", h = BTN_H, on_event = function(player, ctx)
                local dlg = create_settings_dlg()
                dlg:set_parent(ctx.fstk_dialog)
                ctx.fstk_dialog:hide()
                dlg:show()
                ui.update()
            end},
            gui.Button{ label = S"About", h = BTN_H },
        },

        gui.Spacer{},
        gui.Spacer{},
    }
end)

return home
