-- data/scripts/store/handlers.lua

local repository = require("data/scripts/store/repository")
local events = require("data/scripts/store/events")

local handlers = {}

function handlers.gameStoreFetch(player, gameStore, donationUrl, codeGameStore)
    local sex = player:getSex()

    player:sendExtendedOpcode(codeGameStore, json.encode({
        action = "fetchBase",
        data = {
            categories = gameStore.categories,
            url = donationUrl
        }
    }))

    for category, offersTable in pairs(gameStore.offers) do
        local offers = {}

        for _, offer in ipairs(offersTable) do
            local data = {
                type = offer.type,
                title = offer.title,
                description = offer.description,
                price = offer.price
            }

            if offer.count then data.count = offer.count end
            if offer.clientId then data.clientId = offer.clientId end

            if sex == PLAYERSEX_MALE and offer.outfitMale then
                data.outfit = offer.outfitMale
            elseif offer.outfitFemale then
                data.outfit = offer.outfitFemale
            end

            if offer.data then data.data = offer.data end
            table.insert(offers, data)
        end

        player:sendExtendedOpcode(codeGameStore, json.encode({
            action = "fetchOffers",
            data = { category = category, offers = offers }
        }))
    end

    events.gameStoreUpdatePoints(player, codeGameStore)
    events.gameStoreUpdateHistory(player, codeGameStore)
end

function handlers.gameStorePurchase(player, offer, gameStore, codeGameStore)
    local categoryOffers = gameStore.offers[offer.category]
    if not categoryOffers then
        return handlers.errorMsg(player, "Something went wrong, try again or contact server admin [#1]!", codeGameStore)
    end

    for _, offerData in ipairs(categoryOffers) do
        if offerData.title == offer.title and offerData.price == offer.price then
            if not offerData.callback then
                return handlers.errorMsg(player, "Something went wrong, try again or contact server admin [#2]!", codeGameStore)
            end

            local points = repository.getPoints(player)
            if offerData.price > points then
                return handlers.errorMsg(player, "You don't have enough points!", codeGameStore)
            end

            local status = offerData.callback(player, offerData)
            if status ~= true then
                return handlers.errorMsg(player, status, codeGameStore)
            end

            repository.updatePremiumPoints(player, offerData.price)
            repository.insertShopHistory(player, offerData)
            addEvent(events.gameStoreUpdateHistory, 1000, player:getId(), codeGameStore)
            addEvent(events.gameStoreUpdatePoints, 1000, player:getId(), codeGameStore)

            return handlers.infoMsg(player, "You've bought " .. offerData.title .. "!", true, codeGameStore)
        end
    end

    return handlers.errorMsg(player, "Something went wrong, try again or contact server admin [#4]!", codeGameStore)
end

function handlers.errorMsg(player, msg, codeGameStore)
    player:sendExtendedOpcode(codeGameStore, json.encode({
        action = "msg",
        data = { type = "error", msg = msg }
    }))
end

function handlers.infoMsg(player, msg, close, codeGameStore)
    player:sendExtendedOpcode(codeGameStore, json.encode({
        action = "msg",
        data = { type = "info", msg = msg, close = close or false }
    }))
end

return handlers
