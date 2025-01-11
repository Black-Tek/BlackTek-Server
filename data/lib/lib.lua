-- Core API functions implemented in Lua
dofile('data/lib/core/core.lua')

-- Compatibility library for our old Lua API
dofile('data/lib/compat/compat.lua')

-- Debugging helper function for Lua developers
dofile('data/lib/debugging/dump.lua')
dofile('data/lib/debugging/lua_version.lua')
dofile('data/lib/json.lua')

-- Custom
dofile('data/lib/core/crafting.lua')
dofile('data/lib/core/modal_window_lib.lua')
dofile('data/lib/core/vocation_id.lua')
dofile('data/lib/core/ancestral_task.lua')
