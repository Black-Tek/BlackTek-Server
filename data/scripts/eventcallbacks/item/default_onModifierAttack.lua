local ec = EventCallback

-- This event is only called if the modifier damage type and origin type matches the outgoing damage type.

-- item.onModifierAttack(item, player, creature, modifier, damage)
ec.onModifierAttack = function(self, itemHolder, defender, modifier, damage)
    local charges = self:getCharges()
    print("Charges: " .. charges)
    if not charges then
        return true
    end
    print("Augment " .. self:getName() .. " on " .. itemHolder:getName() .. " triggered onModifierAttack.")
    if charges > 1 then
        self:setAttribute(ITEM_ATTRIBUTE_CHARGES, self:getCharges() - 1)
        itemHolder:sendTextMessage(MESSAGE_INFO, "Your " .. self:getName() .. " augment consumed a charge.")
    elseif charges == 1 then
        self:remove(1)
        itemHolder:sendTextMessage(MESSAGE_INFO, "Your " .. self:getName() .. " augment was destroyed.")
    end

    return true
end

ec:register()
