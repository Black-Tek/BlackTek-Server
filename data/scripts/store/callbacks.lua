local callbacks = {}

function callbacks.defaultItemCallback(player, offer)
    local weight = ItemType(offer.itemId):getWeight(offer.count)
    if player:getFreeCapacity() < weight then
        return "This item is too heavy for you!"
    end

    local item = player:getSlotItem(CONST_SLOT_BACKPACK)
    if not item then
        return "You don't have enough space in backpack."
    end

    local slots = item:getEmptySlots(true)
    if slots <= 0 then
        return "You don't have enough space in backpack."
    end

    if player:addItem(offer.itemId, offer.count, false) then
        return true
    end

    return "Something went wrong, item couldn't be added."
end

function callbacks.defaultOutfitCallback(player, offer)
    if offer.outfitMale.addons > 0 then
        if player:hasOutfit(offer.outfitMale.type, offer.outfitMale.addons) then
            return "You already have this outfit with addons."
        end
        player:addOutfitAddon(offer.outfitMale.type, offer.outfitMale.addons)
    else
        if player:hasOutfit(offer.outfitMale.type) then
            return "You already have this outfit."
        end
        player:addOutfit(offer.outfitMale.type)
    end

    if offer.outfitFemale.addons > 0 then
        player:addOutfitAddon(offer.outfitFemale.type, offer.outfitFemale.addons)
    else
        player:addOutfit(offer.outfitFemale.type)
    end
    return true
end

function callbacks.defaultMountCallback(player, offer)
    if player:hasMount(offer.mount) then
        return "You already have this mount."
    end

    player:addMount(offer.mount)
    return true
end

return callbacks
