local gui = flow.widgets
local S = fgettext_ne

local game = flow.make_gui(function(player, ctx)
    assert(ctx.game)

    local size = core.get_window_info().max_formspec_size
    local ENABLE_TOUCH = core.settings:get_bool("enable_touch")

    local BTN_H = 1

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
    }
end)

return game
