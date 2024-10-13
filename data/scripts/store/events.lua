local events = {}
local repository = require("data/scripts/store/repository")

function events.gameStoreUpdateHistory(player, CODE_GAMESTORE)
    if type(player) == "number" then
        player = Player(player)
    end

    local history = {}
    local resultId = repository.fetchShopHistory(player)

    if resultId then
        repeat
            local desc = "Bought " .. result.getDataString(resultId, "title")
            local count = result.getDataInt(resultId, "quantity")
            if count > 0 then
                desc = desc .. " (x" .. count .. ")"
            end
            local target = result.getDataString(resultId, "target")
            if target ~= "" then
                desc =
                    desc .. " on " .. result.getDataString(resultId, "purchase_date") .. " for " .. target .. " for " ..
                        result.getDataInt(resultId, "price") .. " points."
            else
                desc = desc .. " on " .. result.getDataString(resultId, "purchase_date") .. " for " ..
                           result.getDataInt(resultId, "price") .. " points."
            end
            table.insert(history, desc)
        until not result.next(resultId)
        result.free(resultId)
    end

    player:sendExtendedOpcode(CODE_GAMESTORE, json.encode({
        action = "history",
        data = history
    }))
end

function events.gameStoreUpdatePoints(player, CODE_GAMESTORE)
    if type(player) == "number" then
        player = Player(player)
    end
    player:sendExtendedOpcode(CODE_GAMESTORE, json.encode({
        action = "points",
        data = repository.getPoints(player)
    }))
end

function events.refreshPlayersPoints(CODE_GAMESTORE)
    for _, p in ipairs(Game.getPlayers()) do
        if p:getIp() > 0 then
            events.gameStoreUpdatePoints(p, CODE_GAMESTORE)
        end
    end
    addEvent(function()
        events.refreshPlayersPoints(CODE_GAMESTORE)
    end, 10 * 1000)
end

return events
