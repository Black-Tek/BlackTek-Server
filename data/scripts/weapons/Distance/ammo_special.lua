-- Burst arrow
local area = createCombatArea({
    { 1, 1, 1 },
    { 1, 3, 1 },
    { 1, 1, 1 }
})

local combat = Combat()
combat:setParameter(COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE)
combat:setParameter(COMBAT_PARAM_EFFECT, CONST_ME_EXPLOSIONAREA)
combat:setParameter(COMBAT_PARAM_DISTANCEEFFECT, CONST_ANI_BURSTARROW)
combat:setParameter(COMBAT_PARAM_BLOCKARMOR, true)
combat:setFormula(COMBAT_FORMULA_SKILL, 0, 0, 1, 0)
combat:setOrigin(ORIGIN_RANGED)
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
combat:setParameter(COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE)
combat:setParameter(COMBAT_PARAM_DISTANCEEFFECT, CONST_ANI_POISONARROW)
combat:setParameter(COMBAT_PARAM_BLOCKARMOR, true)
combat:setFormula(COMBAT_FORMULA_SKILL, 0, 0, 1, 0)
combat:setOrigin(ORIGIN_RANGED)

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
