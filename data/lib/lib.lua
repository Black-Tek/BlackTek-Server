-- Core API functions implemented in Lua
dofile('data/lib/core/core.lua')

-- Compatibility library for our old Lua API
dofile('data/lib/compat/compat.lua')

-- Debugging helper function for Lua developers
dofile('data/lib/debugging/dump.lua')

-- Area constants and spell helpers must be available before scripts/lib/combats/ loads
dofile('data/scripts/lib/spell_lib.lua')
