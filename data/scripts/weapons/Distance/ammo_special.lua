-- Burst arrow
local area = createCombatArea({
    { 1, 1, 1 },
    { 1, 3, 1 },
    { 1, 1, 1 }
})

local combat = Combat()
combat:setDamageType(Combat.DamageType.Physical)
combat:setImpactEffect(CONST_ME_EXPLOSIONAREA)
combat:setDistanceEffect(CONST_ANI_BURSTARROW)
combat:setBlockedByArmor(true)
combat:setFormula(COMBAT_FORMULA_SKILL, 0, 0, 1, 0)
combat:setOrigin(Combat.Origin.Ranged)
combat:setArea(area)

local burstArrow = Weapon(WEAPON_AMMO)

burstArrow.onUseWeapon = function(player, variant)
    if player:getSkull() == SKULL_BLACK then
        return false
    end

    return combat:execute(player, variant)
end

burstArrow:id(2546)
burstArrow:action("removecount")
burstArrow:register()

-- Poison arrow
local combat = Combat()
combat:setDamageType(Combat.DamageType.Physical)
combat:setDistanceEffect(CONST_ANI_POISONARROW)
combat:setBlockedByArmor(true)
combat:setFormula(COMBAT_FORMULA_SKILL, 0, 0, 1, 0)
combat:setOrigin(Combat.Origin.Ranged)

local poisonArrow = Weapon(WEAPON_AMMO)

poisonArrow.onUseWeapon = function(player, variant)
    if not combat:execute(player, variant) then
        return false
    end

    player:addDamageCondition(Creature(variant:getNumber()), CONDITION_POISON, DAMAGELIST_LOGARITHMIC_DAMAGE, 3)
    return true
end

poisonArrow:id(2545)
poisonArrow:action("removecount")
poisonArrow:register()
