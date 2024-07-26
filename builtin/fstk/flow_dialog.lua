local fake_player = {
    get_player_name = function() return "mainmenu_player" end,
}

core.get_player_by_name = function(name)
    assert(name == "mainmenu_player")
    return fake_player
end

function flow_dialog_create(name, flow_gui, flow_ctx)
    local formspec, process_events, info = flow_gui:render_to_formspec_string(fake_player, flow_ctx)

    local function get_formspec()
        local prepend = ("formspec_version[%d]size[%f,%f]padding[0,0]"):format(
                info.formspec_version, info.w, info.h)
        return prepend .. formspec
    end
    local function buttonhandler(self, fields)
        if process_events(fields) then
            formspec, process_events, info = flow_gui:render_to_formspec_string(fake_player, flow_ctx)
            return true
        end
        return false
    end
    local function eventhandler(event)
        -- TODO: can probably be removed when adding automatic refreshing on window info changes
        if event == "DialogShow" then
            formspec, process_events, info = flow_gui:render_to_formspec_string(fake_player, flow_ctx)
            return true
        end
        return false
    end

    local fstk_dialog = dialog_create(name, get_formspec, buttonhandler, eventhandler)
    flow_ctx.fstk_dialog = fstk_dialog

    return fstk_dialog
end
