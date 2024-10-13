local common_items = {{2460, 1}, -- brass helmet
{2465, 1}, -- brass armor
{2478, 1}, -- brass legs
{2643, 1}, -- leather boots
{1988, 1}, -- backpack
{2120, 1}, -- rope
{2554, 1}, -- shovel
{6119, 1}, -- temple teleporter
{2326, 1}, -- house teleporter
{2789, 100}, -- 100 brown mushroom
{2160, 5}, -- 5 crystal coins
{7618, 5}, -- 5 small health potions
{7620, 5} -- 5 small mana potions
}

local vocation_items = {
    [0] = {}, -- No vocation
    [1] = {{2175, 1}, {2190, 1}}, -- Sorcerer
    [2] = {{2175, 1}, {2182, 1}}, -- Druid
    [3] = {{2456, 1}, {12425, 1}, {2544, 100}, {2525, 1}, {2389, 1}}, -- Paladin
    [4] = {{2525, 1}, {2428, 1}, {2383, 1}, {2439, 1}} -- Knight
}

function onLogin(player)
    if player:getLastLoginSaved() == 0 then
        local vocation = player:getVocation():getId()

        for _, item in ipairs(common_items) do
            local item_id, count = item[1], item[2]
            player:addItem(item_id, count)
        end

        local items = vocation_items[vocation]
        if items then
            for _, item in ipairs(items) do
                local item_id, count = item[1], item[2]
                player:addItem(item_id, count)
            end
        end
    end
    return true
end
