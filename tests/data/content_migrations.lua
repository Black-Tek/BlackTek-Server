-- Run from the repository root: lua tests/data/content_migrations.lua
-- SQL failures must stop the version advance, while partial upgrades can retry.

dofile("data/migrations/0.lua")
local function database(existing, failAt)
    local queries, freed = {}, 0
    db = {
        storeQuery = function(query)
            local tableName = assert(query:match("TABLE_NAME = '([^']+)'"))
            return existing[tableName] and tableName or false
        end,
        query = function(query)
            queries[#queries + 1] = query
            if #queries == failAt then return false end
            local tableName = query:match("ALTER TABLE `([^`]+)`")
            if tableName then existing[tableName] = true end
            return true
        end,
    }
    result = {free = function() freed = freed + 1 end}
    return queries, function() return freed end
end

-- Fail CREATE or each individual ALTER, then retry the partially applied change.
for failAt = 1, 6 do
    local existing = {}
    local queries = database(existing, failAt)
    assert(onUpdateDatabase() == false, "A failed query must not report success")
    assert(#queries == failAt, "Migration must stop after its first failed mutation")
    local retryQueries = database(existing)
    assert(onUpdateDatabase() == true, "A partial upgrade must be retryable")
    assert(#retryQueries == 6 - math.max(0, failAt - 2))
end

local existing = {}
local queries = database(existing)
assert(onUpdateDatabase() == true and #queries == 6)
local repeatQueries, freed = database(existing)
assert(onUpdateDatabase() == true and #repeatQueries == 1)
assert(freed() == 5, "Existing-column query results must be released")
print("PASS: all six SQL failure positions, partial retries, and idempotent migration")
