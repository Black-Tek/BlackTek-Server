local ancestralSystem = CreatureEvent("ancestralTask")

function ancestralSystem.onKill(player, target)
    if target:isPlayer() or target:getMaster() then
        return true
    end

    local mon_name = target:getName():lower()

	function updateTaskProgress(player, taskInfo)
		if taskInfo and (mon_name == taskInfo.name or isInArray(taskInfo.mons_list, mon_name)) then
			local sto_value = player:getStorageValue(taskInfo.storage)
			if sto_value < taskInfo.amount then
				sto_value = sto_value + 1
				player:setStorageValue(taskInfo.storage, sto_value)
				if sto_value < taskInfo.amount then
					player:sendTextMessage(MESSAGE_EVENT_ADVANCE, '[' .. taskInfo.taskType .. ' Ancestral Task] Killed [' .. sto_value .. '/' .. taskInfo.amount .. '] ' .. mon_name .. '.')
				else
					player:sendTextMessage(MESSAGE_EVENT_ADVANCE, '[' .. taskInfo.taskType .. ' Ancestral Task] You finished your ' .. mon_name .. ' task.')
				end
			end
		end
	end
    updateTaskProgress(player, getTaskInfos(player))
    updateTaskProgress(player, getTaskDailyInfo(player))

    local party = player:getParty()
    if party then
        local members = party:getMembers()
        for _, member in ipairs(members) do
            if member:getId() ~= player:getId() then
                updateTaskProgress(member, getTaskInfos(player))
                updateTaskProgress(member, getTaskDailyInfo(player))
            end
        end
    end

    return true
end

ancestralSystem:register()
