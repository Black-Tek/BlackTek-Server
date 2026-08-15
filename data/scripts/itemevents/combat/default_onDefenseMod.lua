local augmentChargeDrainDefense = ItemEvent()
-- When attacker is not a creature it is nil

-- item.onDefenseMod(item, player, creature, modifier, damage)
augmentChargeDrainDefense.onDefenseMod = function(self, itemHolder, attacker, modifier, damage)
    local charges = self:getCharges()

    -- print("Charges: " .. charges)
    if not charges then
        return true
    end

    -- print("Augment " .. self:getName() .. " on " .. itemHolder:getName() .. " triggered onDefenseMod.")
    if charges > 1 then
        self:setAttribute(ITEM_ATTRIBUTE_CHARGES, self:getCharges() - 1)
        itemHolder:sendTextMessage(MESSAGE_INFO_DESCR, "Your " .. self:getName() .. " augment protected you and consumed a charge.")
    elseif charges == 1 then
        self:remove(1)
        itemHolder:sendTextMessage(MESSAGE_INFO_DESCR, "Your " .. self:getName() .. " augment protected you and the item bearing it was destroyed.")
    end

    return true
end

augmentChargeDrainDefense:register(RegisterType.Any)
