local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)
local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid) npcHandler:onCreatureAppear(cid) end
function onCreatureDisappear(cid) npcHandler:onCreatureDisappear(cid) end
function onCreatureSay(cid, type, msg) npcHandler:onCreatureSay(cid, type, msg) end
function onThink() npcHandler:onThink() end
function onPlayerCloseChannel(cid) npcHandler:onPlayerCloseChannel(cid) end

npcHandler:addModule(FocusModule:new())

function creatureSayCallback(cid, type, msg)
    if(not npcHandler:isFocused(cid)) then
        return false
    end
local player = Player(cid)
local msg = msg:lower()
------------------------------------------------------------------
if npcHandler.topic[cid] == 0 and msg == 'normal' then
    npcHandler:say("Great. Which monster task would you like to do ? Write the name of the task.", cid)
    npcHandler.topic[cid] = 1
elseif npcHandler.topic[cid] == 1 then
    if player:getStorageValue(task_sto_time) < os.time() then
        if player:getStorageValue(task_storage) == -1 then
            for mon, l in ipairs(task_monsters) do
                if msg == l.name then
                    npcHandler:say("Okay, you're now doing the {"..l.name:gsub("^%l", string.upper).."} task,  you need to kill "..l.amount.." of them. Come back when you finish!", cid)
                    player:setStorageValue(task_storage, mon)
                    player:setStorageValue(l.storage, 0)
                    npcHandler.topic[cid] = 0
                    break
                elseif mon == #task_monsters then
                    npcHandler:say("Sorry but we don't have this task. Please check the {normal ancestral task list} or the {daily ancestral task list}", cid)
                    npcHandler.topic[cid] = 0
                end
            end
        else
            npcHandler:say("You're already doing a task. You can only do one at a time. Say {!ancestral} to see information about your current task.", cid)
            npcHandler.topic[cid] = 0
        end
    else
        npcHandler:say("I'm not allowed to give you any task because you have abandoned the previous one. Wait for the "..task_time.." hours of punishment to end.", cid)
        npcHandler.topic[cid] = 0
    end
elseif npcHandler.topic[cid] == 0 and msg == 'daily' then
    if player:getStorageValue(time_daySto) < os.time() then
        npcHandler:say("Remember, it is of great importance that the daily tasks are done. Now tell me, which monster task would you like to do? Write the name of the task.", cid)
        npcHandler.topic[cid] = 2
    else
        npcHandler:say('You have completed a daily task today, expect to spend as 24 hours to do it again. You can do {normal} task and check the list here: {normal ancestral tasks list}.', cid)
        npcHandler.topic[cid] = 0
    end
elseif npcHandler.topic[cid] == 2 then
    if player:getStorageValue(task_sto_time) < os.time() then
        if player:getStorageValue(taskd_storage) == -1 then
            for mon, l in ipairs(task_daily) do
                if msg == l.name then
                    npcHandler:say("Very well, you're now doing the daily {"..l.name:gsub("^%l", string.upper).."} task, you need to kill "..l.amount.." of them. Come back when you finish!", cid)
                    player:setStorageValue(taskd_storage, mon)
                    player:setStorageValue(l.storage, 0)
                    npcHandler.topic[cid] = 0
                    break
                elseif mon == #task_daily then
                    npcHandler:say("Sorry we don't have this daily task. Please check the list {daily ancestral task list}.", cid)
                    npcHandler.topic[cid] = 0
                end
            end
        else
            npcHandler:say("You're already doing a daily task. You can only do one per day. Say {!ancestral} to see information about your current task.", cid)
            npcHandler.topic[cid] = 0
        end
    else
        npcHandler:say("I'm not allowed to give you any task because you have abandoned the previous one. Wait for the "..task_time.." hours of punishment to end.", cid)
        npcHandler.topic[cid] = 0
    end
elseif msg == 'reward' then
    if npcHandler.topic[cid] == 0 then
        npcHandler:say("What kind of task did you finish, {normal} or {daily} ?", cid)
        npcHandler.topic[cid] = 3
    end
elseif npcHandler.topic[cid] == 3 then
    if msgcontains(msg, 'normal') then
    local ret_t = getTaskInfos(player)
        if ret_t then
            if player:getStorageValue(ret_t.storage) == ret_t.amount then
                local pt1 = ret_t.pointsTask[1]
                local pt2 = ret_t.pointsTask[2]
                local txt = 'Thanks for finishing the task, your rewards are: '..(pt1 > 1 and pt1..' ancestral points' or pt1 <= 1 and pt1..' ancestral point')..' and '..(pt2 > 1 and pt2..' ancestral rank points' or pt2 <= 1 and pt2..' ancestral rank point')..', '
                if #getItemsFromTable(ret_t.items) > 0 then
                    txt = txt..'in addition: '..getItemsFromTable(ret_t.items)..', '
                for g = 1, #ret_t.items do
                    player:addItem(ret_t.items[g].id, ret_t.items[g].count)
                end
                end

                local exp = ret_t.exp
                if exp > 0 then
                    txt = txt..'I will also give you '..exp..' experience, '
                    player:addExperience(exp)
                end

                taskPoints_add(player, pt1)
                taskRank_add(player, pt2)
                player:setStorageValue(ret_t.storage, -1)
                player:setStorageValue(task_storage, -1)
                npcHandler:say(txt..'thanks again and see you again!', cid)
                npcHandler.topic[cid] = 0
            else
                npcHandler:say('You have not finished your current task yet. You will receive it when you finish.', cid)
                npcHandler.topic[cid] = 0
            end
        else
            npcHandler:say("You aren't doing any tasks. Please check the list {normal ancestral task list} or {daily ancestral task list} in order to start a task and ask me for a {normal} or {daily} ancestral task with the {task name} that is showed on the list related to the task you want to do.", cid)
            npcHandler.topic[cid] = 0
        end
    elseif npcHandler.topic[cid] == 3 and msg == 'daily' then
        if player:getStorageValue(time_daySto)-os.time() <= 0 then
        local ret_td = getTaskDailyInfo(player)
            if ret_td then
                if getTaskDailyInfo(player) then
                    if player:getStorageValue(getTaskDailyInfo(player).storage) == getTaskDailyInfo(player).amount then
                    local pt1 = getTaskDailyInfo(player).pointsTask[1]
                    local pt2 = getTaskDailyInfo(player).pointsTask[2]
                    local txt = 'Thanks for finishing the task, your rewards are: '..(pt1 > 1 and pt1..' ancestral points' or pt1 <= 1 and pt1..' ancestral point')..' e '..(pt2 > 1 and pt2..' ancestral rank points' or pt2 <= 1 and pt2..' ancestral rank point')..', '
                        if #getTaskDailyInfo(player).items > 0 then
                            txt = txt..'in addition: '..getItemsFromTable(getTaskDailyInfo(player).items)..', '
                        for g = 1, #getTaskDailyInfo(player).items do
                            player:addItem(getTaskDailyInfo(player).items[g].id, getTaskDailyInfo(player).items[g].count)
                        end
                        end
                        local exp = getTaskDailyInfo(player).exp
                        if exp > 0 then
                            txt = txt..'I will also give you '..exp..' experience, '
                            player:addExperience(exp)
                        end
                        npcHandler:say(txt..' thanks again and see you again!', cid)
                        taskPoints_add(player, pt1)
                        taskRank_add(player, pt2)
                        player:setStorageValue(getTaskDailyInfo(player).storage, -1)
                        player:setStorageValue(taskd_storage, -1)
                        player:setStorageValue(time_daySto, 1*60*60*24+os.time())
                        npcHandler.topic[cid] = 0
                    else
                        npcHandler:say('You have not finished your current task yet. You will receive it when you finish.', cid)
                        npcHandler.topic[cid] = 0
                    end
                else
                    npcHandler:say("You aren't doing any tasks. Please check the list {normal ancestral task list} or {daily ancestral task list} in order to start a task and ask me for a {normal} or {daily} ancestral task.", cid)
                    npcHandler.topic[cid] = 0
                end
            end
        else
            npcHandler:say("You've done a daily task, wait 24 hours to do another one again, you can do a {normal} ancestral task, check {normal ancestral task list} if needed.", cid)
            npcHandler.topic[cid] = 0
        end
    end

elseif msg == 'abandon' then
    if npcHandler.topic[cid] == 0 then
        npcHandler:say("What kind of task do you want to quit, {normal} or {daily}?", cid)
        npcHandler.topic[cid] = 4
    end
elseif npcHandler.topic[cid] == 4 and msgcontains(msg, 'normal') then
    local ret_t = getTaskInfos(player)
    if ret_t then
        npcHandler:say('Unfortunate this situation, had faith that you would bring me this task done, but I was wrong. As punishment will be '..task_time..' hours without being able to do again a task.', cid)
        player:setStorageValue(task_sto_time, os.time()+task_time*60*60)
        player:setStorageValue(ret_t.storage, -1)
        player:setStorageValue(task_storage, -1)
        npcHandler.topic[cid] = 0
    else
        npcHandler:say("You're not doing any task to be able to abandon it.", cid)
        npcHandler.topic[cid] = 0
    end
elseif npcHandler.topic[cid] == 4 and msg == 'daily' then
    local ret_td = getTaskDailyInfo(player)
    if ret_td then
        npcHandler:say('Unfortunate this situation, had faith that you would bring me this task done, but I was wrong. As punishment will be '..task_time..' hours without being able to do again a task.', cid)
        player:setStorageValue(task_sto_time, os.time()+task_time*60*60)
        player:setStorageValue(ret_td.storage, -1)
        player:setStorageValue(taskd_storage, -1)
        npcHandler.topic[cid] = 0
    else
        npcHandler:say("You're not doing any daily tasks to be able to abandon it.", cid)
        npcHandler.topic[cid] = 0
    end
elseif msg == "normal ancestral task list" then
        local text = "          Normal Ancestral Tasks               \nTo start a Task talk with Armin and say                    the Task name.\n\n"
        for _, d in pairs(task_monsters) do
            text = text .. "Task Name: [" .. d.name .. "]\nMonsters: " .. d.monsters .. ".\n- Amount: [" .. (player:getStorageValue(d.storage) + 1) .. "/" .. d.amount .. "]\n- Ancestral Points: " .. d.pointsTask[1] .. "\n- Ancestral Rank Points: " .. d.pointsTask[2] .. "\n- Reward: " .. (#d.items > 1 and getItemsFromTable(d.items) or "\n") .. "\n- Experience: " .. d.exp .. " points.\n\n"
        end
    
        player:showTextDialog(26643, "" .. text)
        npcHandler:say("Here is the list of normal ancestral tasks. To start a task, ask me {normal} and the {task name} that is related to the task, then you will start doing the task.", cid)
elseif msg == "daily ancestral task list" then
        local text = "              Daily Ancestral Tasks               \n\n"
        for _, d in pairs(task_daily) do
            local itemReward = (#d.items > 0 and getItemsFromTable(d.items) or "\n")
            text = text .. "Task Name: [" .. d.name .. "]\nMonsters: " .. d.monsters .. ".\n- Amount: [" .. (player:getStorageValue(d.storage) + 1) .. "/" .. d.amount .. "]\n- Ancestral Points: " .. d.pointsTask[1] .. "\n- Ancestral Rank Points: " .. d.pointsTask[2] .. "\n- Reward: " .. itemReward  .. "\n- Experience: " .. d.exp .. " points.\n\n"
        end
    
        player:showTextDialog(26643, "" .. text)
        npcHandler:say("Here is the daily ancestral tasks list. To start a daily task, ask me {daily} and the {task name} that is related to the daily task, then you will start doing the task.", cid)
    end
    
end

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)