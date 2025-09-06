local multispawn = TalkAction("/multispawn")
local multisummon = TalkAction("/multisummon")
local Range = {}

Range[1] = {
    {-1, 1},{ 0, 1},{ 1, 1},
    {-1, 0},{ 0, 0},{ 1, 0},
    {-1,-1},{ 0,-1},{ 1,-1}
}

Range[2] = {
    {-2, 2},{-1, 2},{ 0, 2},{ 1, 2},{ 2, 2},
    {-2, 1},{-1, 1},{ 0, 1},{ 1, 1},{ 2, 1},
    {-2, 0},{-1, 0},{ 0, 0},{ 1, 0},{ 2, 0},
    {-2,-1},{-1,-1},{ 0,-1},{ 1,-1},{ 2,-1},
    {-2,-2},{-1,-2},{ 0,-2},{ 1,-2},{ 2,-2},
}

Range[3] = {
    {-3, 3},{-2, 3},{-1, 3},{ 0, 3},{ 1, 3},{ 2, 3},{ 3, 3},
    {-3, 2},{-2, 2},{-1, 2},{ 0, 2},{ 1, 2},{ 2, 2},{ 3, 2},
    {-3, 1},{-2, 1},{-1, 1},{ 0, 1},{ 1, 1},{ 2, 1},{ 3, 1},
    {-3, 0},{-2, 0},{-1, 0},{ 0, 0},{ 1, 0},{ 2, 0},{ 3, 0},
    {-3,-1},{-2,-1},{-1,-1},{ 0,-1},{ 1,-1},{ 2,-1},{ 3,-1},
    {-3,-2},{-2,-2},{-1,-2},{ 0,-2},{ 1,-2},{ 2,-2},{ 3,-2},
    {-3,-3},{-2,-3},{-1,-3},{ 0,-3},{ 1,-3},{ 2,-3},{ 3,-3},
}

local function tableShuffle(t)
    for i = #t, 2, -1 do
        local j = math.random(i)
        t[i], t[j] = t[j], t[i]
    end
    return t
end

function multispawn.onSay(player, words, param)
    if not player:getGroup():getAccess() then
        return true
    end
    local args = param:splitTrimmed(":")
    local creature = args[1]

    if not creature then
        return false
    end

    local player_pos = player:getPosition()
    local doSpawn = function(dataset)
        local x = dataset[1]
        local y = dataset[2]
        if not (x == 0 and y == 0) then
            local target_position = Position(player_pos.x + x, player_pos.y + y, player_pos.z)
            local monster = Game.createMonster(creature, target_position)
        end
    end

    if not args[2] then
        for each, dataset in ipairs(Range[1]) do
            doSpawn(dataset)
        end
        return true
    end

    local rows = tonumber(args[2])


    if rows and rows <= 3 then
        for each, dataset in ipairs(Range[rows]) do
            doSpawn(dataset)
        end
        return true
    end

    local random = args[2] == "/random"
    local count = tonumber(args[3])

    if random and count and count > 0 then
        if count > 48 then
            count = 48
            player:sendTextMessage(MESSAGE_INFO_DESCR, "The max is 48 creatures at one time! Defaulting to the max!")
        end
        local range = tableShuffle(Range[3])
        for each, dataset in ipairs(range) do
            doSpawn(dataset)
            count = (count - 1)
            if count == 0 then break end
        end
        return true
    end
    -- dunno how we got here, lets return false to be safe
    return false
end

multispawn:separator(" ")
multispawn:register()

function multisummon.onSay(player, words, param)

    if not player:getGroup():getAccess() then
        return true
    end

    local args = param:splitTrimmed(":")
    local creature = args[1]

    if not creature then
        return false
    end

    local player_pos = player:getPosition()

    local doSummon = function(dataset)
        local x = dataset[1]
        local y = dataset[2]
        if not (x == 0 and y == 0) then
            local target_position = Position(player_pos.x + x, player_pos.y + y, player_pos.z)
            local monster = Game.createMonster(creature, target_position)
            player:addSummon(monster)
        end
    end

    if not args[2] then
        for each, dataset in ipairs(Range[1]) do
            doSummon(dataset)
        end
        return true
    end

    local rows = tonumber(args[2])

    if rows and rows <=3 then
        for each, dataset in ipairs(Range[rows]) do
            doSummon(dataset)
        end
        return true
    end

    local random = args[2] == "/random"
    local count = tonumber(args[3])

    if random and count and count > 0 then
        if count > 48 then
            count = 48
            player:sendTextMessage(MESSAGE_INFO_DESCR, "The max is 48 creatures at one time! Defaulting to the max!")
        end
        local range = tableShuffle(Range[3])
        for each, dataset in ipairs(range) do
            doSummon(dataset)
            count = (count - 1)
            if count == 0 then break end
        end
        return true
    end
    -- dunno how we got here, lets return false to be safe
    return false
end

multisummon:separator(" ")
multisummon:register()
