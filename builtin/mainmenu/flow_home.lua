local gui = flow.widgets
local S = fgettext_ne


local home = flow.make_gui(function(player, ctx)
    local size = core.get_window_info().max_formspec_size

    return gui.VBox {
        -- bg_fullscreen = "neither",
        min_w = size.x,
        min_h = size.y,

        gui.VBox {
            align_h = "centre",
            align_v = "centre",
            min_w = 5,

            gui.Button{ label = S"Singleplayer", h = 1 },
            gui.Button{ label = S"Multiplayer", h = 1  },
            gui.Button{ label = S"Packages", h = 1  },

            gui.Button{ label = S"Settings", h = 1  },
            gui.Button{ label = S"About", h = 1  },
        },

        gui.Box {align_v = "end", color = "#f00", h = 3},
    }
end)

return home
