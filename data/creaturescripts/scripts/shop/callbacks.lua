-- data/creaturescripts/scripts/shop/callbacks.lua

local Callbacks = {}

-- Default item buy action
function Callbacks.defaultItemBuyAction(player, offer)
    -- todo: add capacity check logic
    if player:addItem(offer.itemId, offer.count, false) then
        return true
    end
    return "Can't add item! Do you have enough space?"
end

-- Default outfit buy action
function Callbacks.defaultOutfitBuyAction(player, offer)
    return "Default outfit buy action is not implemented"
end

-- Default image buy action
function Callbacks.defaultImageBuyAction(player, offer)
    return "Default image buy action is not implemented"
end

-- Custom image buy action
function Callbacks.customImageBuyAction(player, offer)
    return "Custom image buy action is not implemented. Offer: " .. offer.title
end

return Callbacks
