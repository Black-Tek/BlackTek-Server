-- Core API functions implemented in Lua
dofile('data/lib/core/core.lua')

-- Compatibility library for our old Lua API
dofile('data/lib/compat/compat.lua')

-- Debugging helper function for Lua developers
dofile('data/lib/debugging/dump.lua')
dofile('data/lib/debugging/lua_version.lua')

-- Notification services
dofile('data/lib/services/discord.lua')
dofile('data/lib/services/telegram.lua')
dofile('data/lib/services/slack.lua')

