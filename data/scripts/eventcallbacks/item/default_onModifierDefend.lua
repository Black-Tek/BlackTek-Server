local ec = EventCallback

-- This event is only called if the modifier damage type matches the incoming damage type.
-- When attacker is not a creature it is nil

-- item.onModifierDefend(item, player, creature, modifier, damage)
ec.onModifierDefend = function(self, itemHolder, attacker, modifier, damage)
    local charges = self:getCharges()
    print("Charges: " .. charges)
    if not charges then
        return true
    end
    print("Augment " .. self:getName() .. " on " .. itemHolder:getName() .. " triggered onModifierDefend.")
    if charges > 1 then
        self:setAttribute(ITEM_ATTRIBUTE_CHARGES, self:getCharges() - 1)
        itemHolder:sendTextMessage(MESSAGE_INFO_DESCR, "Your " .. self:getName() .. " augment protected you and consumed a charge.")
    elseif charges == 1 then
        self:remove(1)
        itemHolder:sendTextMessage(MESSAGE_INFO_DESCR, "Your " .. self:getName() .. " augment protected you and the item bearing it was destroyed.")
    end

    return true
end

ec:register()
