local spawnTree = GlobalEvent("ressources9")
local itemsList = { 
    {itemId = 26634, quantity = 50},
    {itemId = 26606, quantity = 20},
    {itemId = 26610, quantity = 12},
    {itemId = 26613, quantity = 7},
    {itemId = 26609, quantity = 5},
    {itemId = 26405, quantity = 20},
    {itemId = 26406, quantity = 15},
    {itemId = 26407, quantity = 10},
    {itemId = 26411, quantity = 10},
    {itemId = 26410, quantity = 9},
    {itemId = 26408, quantity = 7},
    {itemId = 26409, quantity = 5},
    {itemId = 26717, quantity = 15},
    {itemId = 26718, quantity = 11},
    {itemId = 26719, quantity = 11},
    {itemId = 26720, quantity = 10},
    {itemId = 26721, quantity = 7},
    {itemid = 26722, quantity = 5}
}

local STARTUP = {}

STARTUP[#STARTUP + 1] = { 
    action = function()
        local fromPos = {x= 32633, y=32591, z=7}
        local toPos = {x = 32920, y = 32723, z = 7}
        for _, item in ipairs(itemsList) do
            for i = 1, item.quantity do
                local pos = {x=math.random(fromPos.x,toPos.x), y=math.random(fromPos.y,toPos.y), z=math.random(fromPos.z,toPos.z)}
                local tile = Tile(pos)
                if tile then
                    local ground = tile:getGround()
                    if ground then
                        local groundId = ground:getId()
                        if groundId < 4608 or groundId > 4619 then
                            local tileInfo = getTileInfo(pos)
                            if tileInfo and tileInfo.items == 0 and tileInfo.creatures == 0 then 
                                addEvent(function() doCreateItem(item.itemId, 1, pos) end, 1000)
                            end
                        end
                    end
                end
            end 
        end
    end
}

function spawnTree.onStartup()
    for _, cmd in pairs(STARTUP) do
        if (cmd.msg) then
            local x = os.clock()
            cmd.action()
        else
            cmd.action()
        end
    end

    STARTUP = nil 
    return true
end

spawnTree:register()
