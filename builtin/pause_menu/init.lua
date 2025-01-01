local scriptpath = core.get_builtin_path()
local clientpath = scriptpath.."client"..DIR_DELIM
local commonpath = scriptpath.."common"..DIR_DELIM

-- we're in-game, so no absolute paths are needed
defaulttexturedir = ""

dofile(clientpath .. "register.lua")
dofile(commonpath .. "settings" .. DIR_DELIM .. "init.lua")
