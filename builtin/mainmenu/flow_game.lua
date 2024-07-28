local gui = flow.widgets
local S = fgettext_ne

local game = flow.make_gui(function(player, ctx)
    assert(ctx.game)

    local size = core.get_window_info().max_formspec_size
    local ENABLE_TOUCH = core.settings:get_bool("enable_touch")

    local BTN_H = 1

	menudata.worldlist:set_filtercriteria(ctx.game.id)

    return gui.VBox {
        bg_fullscreen = "neither",
        min_w = size.x,
        min_h = size.y,

        gui.HBox {
            gui.Button { label = "<", h = BTN_H, on_event = function()
                ctx.fstk_dialog:delete()
                ui.update()
            end},
            gui.Label { label = ctx.game.title, align_h = "center", expand = true, h = BTN_H },
            gui.Button { label = "<", h = BTN_H, visible = false}, -- make the stupid title cebntered
        },
        gui.HBox {
            expand = true,
            gui.Textlist{
                expand = true,
                name = "worldlist", listelems = string.split(menu_render_worldlist(), ","),
                selected_idx = 2
            },
            gui.VBox {
                w = 5,
                gui.Checkbox { name = "host_server", label = "Host Server" },
                gui.Field{ name = "name", label = "Name"},
                gui.Field{ name = "password", label = "Password"},
                gui.Field{name = "port", label = "Port"},
                gui.Button {label = "Play", h = BTN_H},
                gui.HBox {
                    gui.Button{label = "Settings", expand = true},
                    gui.Button {label = "Delete", expand = true},
                }
            },
        }
    }
end)

return game
