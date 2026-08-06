-- Burst arrow
local combat = Combat(WeaponCombats.BurstArrow)

local burstArrow = ItemEvent(WEAPON_AMMO)

burstArrow.onUseAsWeapon = function(player, item, variant)
    if player:getSkull() == SKULL_BLACK then
        return false
    end

    return combat:execute(player, variant)
end

burstArrow:id(2546)
burstArrow:action("removecount")
burstArrow:register()

-- Poison arrow
local combat = Combat(WeaponCombats.PoisonArrow)

local poisonArrow = ItemEvent(WEAPON_AMMO)

poisonArrow.onUseAsWeapon = function(player, item, variant)
    if not combat:execute(player, variant) then
        return false
    end

    player:addDamageCondition(Creature(variant:getNumber()), CONDITION_POISON, DAMAGELIST_LOGARITHMIC_DAMAGE, 3)
    return true
end

poisonArrow:id(2545)
poisonArrow:action("removecount")
poisonArrow:register()
