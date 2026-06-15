-- Viper star
local combat = Combat(WeaponCombats.ViperStar)

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
