local Repository = {}

function Repository.getPoints(player)
    local points = 0
    local resultId = db.storeQuery("SELECT `premium_points` FROM `accounts` WHERE `id` = " .. player:getAccountId())
    if resultId ~= false then
        points = result.getDataInt(resultId, "premium_points")
        result.free(resultId)
    end
    return points
end

function Repository.updatePoints(player, points)
    local accountId = player:getAccountId()
    return db.query("UPDATE `accounts` SET `premium_points` = `premium_points` - " .. points .. " WHERE `id` = " .. accountId)
end

function Repository.logPurchase(player, offer)
    local accountId = player:getAccountId()
    local playerId = player:getGuid()
    local title = db.escapeString(offer['title'])
    local cost = db.escapeString(offer['cost'])
    local details = db.escapeString(json.encode(offer))

    return db.asyncQuery(
        "INSERT INTO `shop_history` (`account`, `player`, `date`, `title`, `cost`, `details`) " ..
        "VALUES ('" .. accountId .. "', '" .. playerId .. "', NOW(), " .. title .. ", " .. cost .. ", " .. details .. ")"
    )
end

function Repository.getPurchaseHistory(player)
    local accountId = player:getAccountId()
    local query = "SELECT * FROM `shop_history` WHERE `account` = " .. accountId .. " ORDER BY `id` DESC"
    local resultId = db.storeQuery(query)

    if not resultId then
        return {}
    end

    local history = {}
    repeat
        local details = result.getDataString(resultId, "details")
        local status, json_data = pcall(function() return json.decode(details) end)

        if not status then
            json_data = {
                type = "image",
                title = result.getDataString(resultId, "title"),
                cost = result.getDataInt(resultId, "cost")
            }
        end

        json_data["description"] = "Bought on " .. result.getDataString(resultId, "date") ..
                                   " for " .. result.getDataInt(resultId, "cost") .. " points."
        table.insert(history, json_data)
    until not result.next(resultId)

    result.free(resultId)
    return history
end

return Repository
