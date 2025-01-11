task_monsters = {
    [1] = {name = "raging demons", mons_list = {"demon"}, storage = 400029, amount = 115, exp = 6500000, pointsTask = {4, 4}, items = {{id = 2472, count = 1},{id = 2470, count = 1}, {id = 7393, count = 1}, {id = 2160, count = 10} }, monsters = "Kill Demons around the world"},
    [2] = {name = "fire dragons", mons_list = {"dragon", "dragon lord"}, storage = 400030, amount = 200, exp = 4000000, pointsTask = {2, 2}, items = {{id = 7399, count = 1}, {id = 7402, count = 1}, {id = 2160, count = 4 }}, monsters = "Kill Dragons and Dragon lords around the world"},
    [3] = {name = "silence mountain", mons_list = {"frazzlemaw", "silencer"}, storage = 400031, amount = 150, exp = 7500000, pointsTask = {3, 2}, items = {{id = 22396, count = 1}, {id = 2160, count = 10}, {id = 18429, count = 10}, {id = 18421, count = 10}}, monsters = "Kill Frazzlemaws and Silencers"},
    [4] = {name = "fear valley", mons_list = {"retching horror", "choking Fear"}, storage = 400032, amount = 150, exp = 8300000, pointsTask = {4, 4}, items = {{id = 22396, count = 2}, {id = 2645, count = 1}, {id = 7412, count = 1}, {id = 7451, count = 1}, {id = 2160, count = 20}}, monsters = "Kill Retching Horrors and Choking Fear"},
    [5] = {name = "poison combo", mons_list = {"hydra", "serpent spawn"}, storage = 400033, amount = 150, exp = 5000000, pointsTask = {3, 2}, items = {{id = 2195, count = 1}, {id = 8880, count = 1}, {id = 2498, count = 1}, {id = 11230, count = 8}, {id = 11199, count = 8}, {id = 2160, count = 6}}, monsters = "Kill Hydras and Serpent Spawn"},
    [6] = {name = "night reapers", mons_list = {"grim reaper", "nightmare"}, storage = 400034, amount = 150, exp = 6150000, pointsTask = {3, 2}, items = {{id = 2160, count = 6}, {id = 8889, count = 5}, {id = 6526, count = 1}, {id = 6500, count = 10}}, monsters = "Kill Grim Reapers and nightmares"},
    [7] = {name = "catacombs", mons_list = {"demon", "dark torturer", "juggernaut", "hellhound", "destroyer", "hellspawn"}, storage = 400035, amount = 250, exp = 18000000, pointsTask = {6, 6}, items = {{id = 2160, count = 30}, {id = 7421, count = 1}, {id = 5801, count = 1}, {id = 2136, count = 1}, {id = 5741, count = 1}, {id = 9970, count = 10}, {id = 2150, count = 10}, {id = 2147, count = 10}}, monsters = "Kill Demons, Hellhounds, Juggernaut, Destroyers, Hellspawn and Dark Torturers"},
    [8] = {name = "wyrms", mons_list = {"wyrm", "elder wyrm"}, storage = 400036, amount = 150, exp = 2375000, pointsTask = {2, 2}, items = {{id = 2160, count = 5},{id = 8855, count = 1}, {id = 7451, count = 1}, {id = 2145, count = 10}}, monsters = "Kill Wyrms and Elder Wyrms"},
    [9] = {name = "minotaurs", mons_list = {"worm priestess", "minotaur amazon", "minotaur hunter", "mooh'tah warrior"}, storage = 400037, amount = 150, exp = 800000, pointsTask = {3, 2}, items = {{id = 2160, count = 5},{id = 5878, count = 30}, {id = 23537, count = 1}, {id = 7401, count = 1}}, monsters = "Kill Wormd Priestess, Minotaur Amazon, Hunter and Mooh'tah warrior"},
   }
 
 task_daily = {
    [1] = {name = "minotaurs", mons_list = {"worm priestess", "minotaur amazon", "minotaur hunter", "mooh'tah warrior"}, storage = 503001, amount = 1, exp = 18500000, pointsTask = {5, 4}, items = {{id = 2160, count = 12}}, monsters = "Kill Wormd Priestess, Minotaur Amazon, Hunter and Mooh'tah warrior"},
} 

 
 task_storage = 480000
 task_sto_time = 480002
 task_time = 1
 taskd_storage = 480004
 time_daySto = 480005
 
 
 local ranks_task = {
 [{1, 40}] = "Rookie", 
 [{41, 100}] = "Bronze",
 [{101, 250}] = "Silver",
 [{251, 470}] = "Gold",
 [{471, 770}] = "Platinum",
 [{771, 1220}] = "Diamond",
 [{1221, 1820}] = "Ancestral",
 [{1821, 2820}] = "Mystic",
 [{2821, 4320}] = "Abyssal",
 [{4321, 6320}] = "Ascending",
 [{6321, 9999}] = "Chaos",
 [{10000, math.huge}] = "Awakening"
 }
 
 local RankSequence = {
 ["Rookie"] = 1,
 ["Bronze"] = 2,
 ["Silver"] = 3,
 ["Gold"] = 4,
 ["Platinum"] = 5,
 ["Diamond"] = 6,
 ["Ancestral"] = 7,
 ["Mystic"] = 8,
 ["Abyssal"] = 9,
 ["Ascending"] = 10,
 ["Chaos"] = 11,
 ["Awakening"] = 12
 }
 
 function rankIsEqualOrHigher(myRank, RankCheck)
     local ret_1 = RankSequence[myRank]
     local ret_2 = RankSequence[RankCheck]
     return ret_1 >= ret_2
 end
 
 function getTaskInfos(player)
    local player = Player(player)
    local taskInfo = task_monsters[player:getStorageValue(task_storage)]
    if taskInfo then
        taskInfo.taskType = "Normal"
    end
    return taskInfo or false
end

function getTaskDailyInfo(player)
    local player = Player(player)
    local taskInfo = task_daily[player:getStorageValue(taskd_storage)]
    if taskInfo then
        taskInfo.taskType = "Daily"
    end
    return taskInfo or false
end
 
 
 function taskPoints_get(player)
    local points = 0
    local accountId = player:getGuid() 
    local resultId = db.storeQuery("SELECT `ancestral_points` FROM `players` WHERE `id` = " .. accountId)
    if resultId ~= false then
        points = result.getDataInt(resultId, "ancestral_points")
        result.free(resultId)
    end
    return points
 end



 function taskPoints_add(player, count)
    local accountId = player:getGuid() 

    db.query(string.format("UPDATE players SET ancestral_points = ancestral_points + %d WHERE id = %d", count, accountId))
end
 
 function taskPoints_remove(player, count)
    local taskPoints = taskPoints_get(player) or 0 
    local accountId = player:getGuid() 

    local newPoints = taskPoints - count
    if newPoints < 0 then
        newPoints = 0
    end

    db.query(string.format("UPDATE players SET ancestral_points = %d WHERE id = %d", newPoints, accountId))
end


 
 function taskRank_get(player)
    local accountId = player:getGuid() 
    local points = 0
    local resultId = db.storeQuery("SELECT `ancestral_rank_points` FROM `players` WHERE `id` = " .. accountId)
    if resultId ~= false then
        points = result.getDataInt(resultId, "ancestral_rank_points")
        result.free(resultId)
    end
    return points
 end
 
 function taskRank_add(player, count)
    local accountId = player:getGuid() 

    db.query(string.format("UPDATE players SET ancestral_rank_points = ancestral_rank_points + %d WHERE id = %d", count, accountId))
end
 
 function getRankTask(player)
     local pontos = taskRank_get(player)
     local ret
     for _, z in pairs(ranks_task) do
         if pontos >= _[1] and pontos <= _[2] then
             ret = z
         end
     end
     return ret
 end
 
 function getItemsFromTable(itemtable)
      local text = ""
      for v = 1, #itemtable do
          count, info = itemtable[v].count, ItemType(itemtable[v].id)
          local ret = "\n   - "
          if v == 1 then
              ret = ""
          elseif v == #itemtable then
              ret = "\n   - "
          end
          text = text .. ret
          text = text .. (count > 1 and count or info:getArticle()).." "..(count > 1 and info:getPluralName() or info:getName())
      end
      return text
 end
