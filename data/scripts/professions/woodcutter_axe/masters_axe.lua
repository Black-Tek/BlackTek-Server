local woodcutterSystem = Action()
local COOLDOWN_SECONDS = 4

local config = {
    [26604] = {text = {"Ash Tree is regenerating"}}, 
    [26605] = {break_chance = 75, loot_chance = 80, lower_tier = 26604, requiredSkill = 5, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26595, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26595, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26595, count = 4},
        },
    },
    [26606] = {break_chance = 75, loot_chance = 80, lower_tier = 26605, requiredSkill = 5, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26595, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26595, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26595, count = 4},
        },
    },
    [26636] = {text = {"Small Tree is regenerating"}}, 
    [26635] = {break_chance = 75, loot_chance = 80, lower_tier = 26636, requiredSkill = 0, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26593, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26594, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26593, count = 4},
        },
    },
    [26634] = {break_chance = 75, loot_chance = 80, lower_tier = 26635, requiredSkill = 0, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26593, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26594, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26593, count = 4},
        },
    },
    [26612] = {text = {"Chestnut Tree is regenerating"}}, 
    [26611] = {break_chance = 75, loot_chance = 80, lower_tier = 26612, requiredSkill = 15, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26596, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26596, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26596, count = 4},
        },
    },
    [26610 ] = {break_chance = 75, loot_chance = 80, lower_tier = 26611, requiredSkill = 15, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26596, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26596, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26596, count = 4},
        },
    },
    [26615] = {text = {"Olive Tree is regenerating"}}, 
    [26614] = {break_chance = 75, loot_chance = 80, lower_tier = 26615, requiredSkill = 25, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26598, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26598, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26598, count = 4},
        },
    },
    [26613 ] = {break_chance = 75, loot_chance = 80, lower_tier = 26614, requiredSkill = 25, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26598, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26598, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26598, count = 4},
        },
    },
    [26607] = {text = {"Cherry Tree is regenerating"}}, 
    [26608] = {break_chance = 75, loot_chance = 80, lower_tier = 26607, requiredSkill = 40, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26597, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26597, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26597, count = 4},
        },
    },
    [26609 ] = {break_chance = 75, loot_chance = 80, lower_tier = 26608, requiredSkill = 40, 
        normal_loot_table = { -- 80%
            [1] = {item_id = 26597, count = 1}, 
        },
        semi_rare_loot_table = { -- 15%
            [1] = {item_id = 26597, count = 2}, 
        },
        rare_loot_table = { -- 5%
            [1] = {item_id = 26597, count = 4},
        },
    },


}

function woodcutterSystem.onUse(cid, item, fromPosition, itemEx, toPosition)
    local player = Player(cid)
    if not player then
        return true
    end
    
    local lastUsageTime = player:getStorageValue(215563) 

    if os.time() - lastUsageTime < COOLDOWN_SECONDS then
        player:sendTextMessage(MESSAGE_INFO_DESCR, "You must wait a few seconds before using the axe again.")
        return true
    end

    local object = config[itemEx.itemid]
    if object then
        if object.text then
            local rand_text = math.random(#object.text)
            doCreatureSay(cid, object.text[rand_text], TALKTYPE_ORANGE_1, nil, nil, toPosition)
            doSendMagicEffect(toPosition, 181)
            return true
        end
        
        doSendMagicEffect(toPosition, 180)  
        doSendMagicEffect(player:getPosition(), 176) 
        addEvent(function()
            local rand_event = math.random(200)
            if rand_event == 200 then
                doSendMagicEffect(toPosition, 181)
                return true
            end

            local rand_break = math.random(100)
            if rand_break <= object.break_chance then
                doTransformItem(itemEx.uid, object.lower_tier)
                doSendMagicEffect(toPosition, 181)
                local transformedItem = Item(itemEx.uid)
                transformedItem:decay()
            end

            local rand_reward = math.random(100)
            if rand_reward > (object.loot_chance + 10) then
                doSendMagicEffect(toPosition, 181)
                return true
            end

            local multiple = 1
            local rand_multiple = math.random(15)
            if rand_multiple == 15 then
                multiple = math.random(2, 3)
            end
            for i = 1, multiple do
                local loot_table = object.normal_loot_table
                local rand_loot = math.random(100)
                if rand_loot < 6 then
                    loot_table = object.rare_loot_table
                elseif rand_loot < 21 then
                    loot_table = object.semi_rare_loot_table
                end
                local rand_item = math.random(#loot_table)
                rand_item = loot_table[rand_item]
                if rand_item.count == 1 then
                    doPlayerAddItem(cid, rand_item.item_id, 1, true)
                else
                    local new_count = math.random(rand_item.count)
                    doPlayerAddItem(cid, rand_item.item_id, new_count, true)
                end
            end
        end, 3000)  
    else
        player:say("This axe cannot cut on this tree.", TALKTYPE_MONSTER_SAY)


    end
    player:setStorageValue(215563, os.time())
    return true
end

woodcutterSystem:id(26592)
woodcutterSystem:register()