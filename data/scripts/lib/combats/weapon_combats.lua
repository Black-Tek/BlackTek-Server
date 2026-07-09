-- Combat table definitions for special-ability weapons.
-- formula = {mina, minb, maxa, maxb} — positional array matching setFormula args 3-6.

WeaponCombats = {

    BurstArrow = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_EXPLOSIONAREA,
        distanceEffect = CONST_ANI_BURSTARROW,
        blockedByArmor = true,
        origin         = Combat.Origin.Ranged,
        formula        = { 0, 0, 1, 0 },
        area           = createCombatArea({ { 1, 1, 1 }, { 1, 3, 1 }, { 1, 1, 1 } }),
    },

    PoisonArrow = {
        damageType     = Combat.DamageType.Physical,
        distanceEffect = CONST_ANI_POISONARROW,
        blockedByArmor = true,
        origin         = Combat.Origin.Ranged,
        formula        = { 0, 0, 1, 0 },
    },

    ViperStar = {
        damageType     = Combat.DamageType.Physical,
        distanceEffect = CONST_ANI_GREENSTAR,
        blockedByArmor = true,
        origin         = Combat.Origin.Ranged,
        formula        = { 0, 0, 1, 0 },
    },
}
