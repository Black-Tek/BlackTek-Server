local spawnTree = GlobalEvent("ressources15")
local itemsList = { 
    {itemId = 26405, quantity = 10},
    {itemId = 26406, quantity = 9},
    {itemId = 26407, quantity = 9},
    {itemId = 26411, quantity = 9},
    {itemId = 26410, quantity = 7},
    {itemId = 26408, quantity = 6},
    {itemId = 26409, quantity = 4},
    {itemId = 26717, quantity = 9},
    {itemId = 26718, quantity = 9},
    {itemId = 26719, quantity = 7},
    {itemId = 26720, quantity = 6},
    {itemId = 26721, quantity = 4},
    {itemid = 26722, quantity = 3}
}

local STARTUP = {}

STARTUP[#STARTUP + 1] = { 
    action = function()
        local fromPos = {x= 33206, y=32284, z=7}
        local toPos = {x = 33292, y = 32380, z = 7}
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
