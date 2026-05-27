-- Viper star
local combat = Combat()
combat:setDamageType(Combat.DamageType.Physical)
combat:setDistanceEffect(CONST_ANI_GREENSTAR)
combat:setBlockedByArmor(true)
combat:setFormula(COMBAT_FORMULA_SKILL, 0, 0, 1, 0)
combat:setOrigin(Combat.Origin.Ranged)

local viperStar = Weapon(WEAPON_DISTANCE)

viperStar.onUseWeapon = function(player, variant)
    if not combat:execute(player, variant) then
        return false
    end

    if math.random(1, 100) <= 90 then
        return false
    end

    player:addDamageCondition(Creature(variant:getNumber()), CONDITION_POISON, DAMAGELIST_LOGARITHMIC_DAMAGE, 2)
    return true
end

viperStar:id(7366)
viperStar:action("removecount")
viperStar:register()
