local gui = flow.widgets
local S = fgettext_ne


local home = flow.make_gui(function(player, ctx)
    local size = core.get_window_info().max_formspec_size

    local HEADER_I_H = 2
    local HEADER_T_H = 1.5

    return gui.VBox {
        bg_fullscreen = "neither",
        min_w = size.x,
        min_h = size.y,

        gui.Spacer{},
        gui.HBox {
            align_h = "center",
            gui.Image { w = HEADER_I_H, h = HEADER_I_H, texture_name = defaulttexturedir .. "/logo.png" },
            gui.Image { w = 127 / 16 * HEADER_T_H, h = HEADER_T_H, texture_name = defaulttexturedir .. "/menu_header.png" },
        },
        gui.Spacer{},

        gui.VBox {
            align_h = "centre",
            align_v = "centre",
            -- replaced by gui.Spacer{}s
            -- expand = true,
            min_w = 5,

            gui.Button{ label = S"Singleplayer", h = 1 },
            gui.Button{ label = S"Multiplayer", h = 1  },
            gui.Button{ label = S"Packages", h = 1  },

            gui.Button{ label = S"Settings", h = 1  },
            gui.Button{ label = S"About", h = 1  },
        },

        gui.Spacer{},
        gui.Spacer{},
    }
end)

return home
