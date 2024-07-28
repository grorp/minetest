local gui = flow.widgets
local S = fgettext_ne

local gamelist = flow.make_gui(function(player, ctx)
    local size = core.get_window_info().max_formspec_size
    -- size.y = size.y + 1.5 -- don't ask, just remove this
    local ENABLE_TOUCH = core.settings:get_bool("enable_touch")

    local BTN_H = 1
    local TILE_W = 4.5
    local TILE_H = TILE_W / 3 * 2

    local row_w = 0
    local finished_rows = {}
    local cells = {}

	for _, game in ipairs(pkgmgr.games) do
        if not game.screenshot_path then
            game.screenshot_path = game.path .. DIR_DELIM .. "screenshot.png"
            local f = io.open(game.screenshot_path, "r")
            if f ~= nil then
                f:close()
            else
                game.screenshot_path = defaulttexturedir .. "no_screenshot.png"
            end
        end

        table.insert(cells, gui.VBox {
            spacing = 0,
            gui.Image { w = TILE_W, h = TILE_H, texture_name = game.screenshot_path },
            gui.Label { label = game.title, lbl_noclip = true, align_h = "center", h = 0.5 },
        })
        row_w = row_w + TILE_W
        if row_w > size.x - 1 then -- TODO: figure out actual padding to subtract
            table.insert(finished_rows, gui.HBox(cells))
            cells = {}
            row_w = 0
        end
	end

    if #cells > 0 then
        table.insert(finished_rows, gui.HBox(cells))
        cells = {}
        row_w = 0
    end

    finished_rows.name = "gamelist"
    -- why the hell do I have to do this manually?
    -- just setting expand=true didn't do anything
    finished_rows.h = size.y - BTN_H - 1

    return gui.VBox {
        bg_fullscreen = "both",
        min_w = size.x,
        min_h = size.y,

        gui.HBox {
            gui.Button { label = "<", h = BTN_H, on_event = function()
                ctx.fstk_dialog:delete()
                ui.update()
            end},
            gui.Label { label = "Games", align_h = "center", expand = true, h = BTN_H },
            gui.Button { label = "<", h = BTN_H, visible = false}, -- make the stupid title cebntered
        },
        gui.PaginatedVBox(finished_rows),
    }
end)

return gamelist
