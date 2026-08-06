local augmentChargeDrainAttack = ItemEvent()

-- This event is only called if the modifier damage type and origin type matches the outgoing damage type.

-- item.onAttackMod(item, player, creature, modifier, damage)
augmentChargeDrainAttack.onAttackMod = function(self, itemHolder, defender, modifier, damage)
    local charges = self:getCharges()
    -- print("Charges: " .. charges)
    if not charges then
        return true
    end

    -- print("Augment " .. self:getName() .. " on " .. itemHolder:getName() .. " triggered onAttackMod.")
    if charges > 1 then
        self:setAttribute(ITEM_ATTRIBUTE_CHARGES, self:getCharges() - 1)
        itemHolder:sendTextMessage(MESSAGE_INFO, "Your " .. self:getName() .. " augment consumed a charge.")
    elseif charges == 1 then
        self:remove(1)
        itemHolder:sendTextMessage(MESSAGE_INFO, "Your " .. self:getName() .. " augment was destroyed.")
    end

    return true
end

augmentChargeDrainAttack:register(RegisterType.Any)
