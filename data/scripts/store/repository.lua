local repository = {}

function repository.getPoints(player)
    local points = 0
    local resultId = db.storeQuery("SELECT `premium_points` FROM `accounts` WHERE `id` = " .. player:getAccountId())
    if resultId then
        points = result.getDataInt(resultId, "premium_points")
        result.free(resultId)
    end
    return points
end

function repository.updatePremiumPoints(player, amount)
    local aid = player:getAccountId()
    db.query("UPDATE `accounts` SET `premium_points` = `premium_points` - " .. amount .. " WHERE `id` = " .. aid)
end

function repository.insertShopHistory(player, offer, targetPlayer)
    local aid = player:getAccountId()
    local escapeTitle = db.escapeString(offer.title)
    local escapePrice = db.escapeString(offer.price)
    local escapeCount = offer.count and db.escapeString(offer.count) or 0
    local escapeTarget = targetPlayer and db.escapeString(targetPlayer:getName()) or "NULL"
    db.asyncQuery("INSERT INTO `shop_history` VALUES (NULL, '" .. aid .. "', '" .. player:getGuid() .. "', NOW(), " ..
                      escapeTitle .. ", " .. escapePrice .. ", " .. escapeCount .. ", " .. escapeTarget .. ")")
end

function repository.fetchShopHistory(player)
    return db.storeQuery("SELECT * FROM `shop_history` WHERE `account_id` = " .. player:getAccountId() ..
                             " ORDER BY `id` DESC")
end

return repository
