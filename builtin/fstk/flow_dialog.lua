local fake_player = {
    get_player_name = function() return "mainmenu_player" end,
}

core.get_player_by_name = function(name)
    assert(name == "mainmenu_player")
    return fake_player
end

function flow_dialog_create(name, flow_gui, flow_ctx)
    local formspec, process_events = flow_gui:render_to_formspec_string(fake_player, flow_ctx, true)

    local function get_formspec()
        return formspec
    end
    local function buttonhandler(self, fields)
        if process_events(fields) then
            formspec, process_events = flow_gui:render_to_formspec_string(fake_player, flow_ctx, true)
            return true
        end
        return false
    end
    local function eventhandler(self, event)
        return false
    end

    return dialog_create(name, get_formspec, buttonhandler, eventhandler)
end
