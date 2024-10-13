-- data/scripts/store/offers.lua

local callbacks = require("data/scripts/store/callbacks")
local offers = {}

function offers.addCategory(gameStore, title, description, iconType, iconData)
    if iconType == "item" then
        iconData = ItemType(iconData):getClientId()
    end

    table.insert(gameStore.categories, {
        title = title,
        description = description,
        iconType = iconType,
        iconData = iconData
    })
end

function offers.addItem(gameStore, category, title, description, itemId, count, price)
    if not gameStore.offers[category] then
        gameStore.offers[category] = {}
    end

    table.insert(gameStore.offers[category], {
        type = "item",
        title = title,
        description = description,
        itemId = itemId,
        count = count,
        price = price,
        clientId = ItemType(itemId):getClientId(),
        callback = callbacks.defaultItemCallback
    })
end

function offers.addOutfit(gameStore, category, title, description, outfitMale, outfitFemale, price)
    if not gameStore.offers[category] then
        gameStore.offers[category] = {}
    end

    table.insert(gameStore.offers[category], {
        type = "outfit",
        title = title,
        description = description,
        outfitMale = outfitMale,
        outfitFemale = outfitFemale,
        price = price,
        callback = callbacks.defaultOutfitCallback
    })
end

function offers.addMount(gameStore, category, title, description, mountId, clientId, price)
    if not gameStore.offers[category] then
        gameStore.offers[category] = {}
    end

    table.insert(gameStore.offers[category], {
        type = "mount",
        title = title,
        description = description,
        mount = mountId,
        clientId = clientId,
        price = price,
        callback = callbacks.defaultMountCallback
    })
end

function offers.addCustom(gameStore, category, type, title, description, data, count, price, callback)
    if not gameStore.offers[category] then
        gameStore.offers[category] = {}
    end

    if not callback then
        error("[Game Store] addCustom " .. title .. " without callback")
        return
    end

    table.insert(gameStore.offers[category], {
        type = type,
        title = title,
        description = description,
        data = data,
        price = price,
        count = count,
        callback = callback
    })
end

return offers
