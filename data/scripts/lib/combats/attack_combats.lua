-- Combat table definitions for player attack spells.
-- Pass an entry to Combat(table) in the spell file to create the combat object.
-- Output formulas are registered per-spell via FormulaNode.random + combat:registerFormula.

AttackCombats = {

    Annihilation = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_WEAPONTYPE,
        blockedByArmor = true,
        useCharges     = true,
    },

    ApprenticesStrike = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
    },

    Berserk = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        blockedByArmor = true,
        useCharges     = true,
        area           = createCombatArea(AREA_SQUARE1X1),
    },

    BrutalStrike = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_WEAPONTYPE,
        blockedByArmor = true,
        useCharges     = true,
    },

    Buzz = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
    },

    ChillOut = {
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_ICEAREA,
        area         = createCombatArea(AREA_WAVE4),
    },

    Curse = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_MORTAREA,
        distanceEffect = CONST_ANI_DEATH,
    },

    DeathStrike = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_MORTAREA,
        distanceEffect = CONST_ANI_DEATH,
    },

    DivineCaldera = {
        damageType   = Combat.DamageType.Holy,
        impactEffect = CONST_ME_HOLYAREA,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    DivineMissile = {
        damageType     = Combat.DamageType.Holy,
        impactEffect   = CONST_ME_HOLYDAMAGE,
        distanceEffect = CONST_ANI_SMALLHOLY,
    },

    Electrify = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
    },

    EnergyBeam = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_ENERGYHIT,
        area         = createCombatArea(AREA_BEAM5, AREADIAGONAL_BEAM5),
    },

    EnergyStrike = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
    },

    EnergyWave = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_SQUAREWAVE5, AREADIAGONAL_SQUAREWAVE5),
    },

    Envenom = {
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_EARTH,
    },

    EternalWinter = {
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_ICETORNADO,
        area         = createCombatArea(AREA_CIRCLE5X5),
    },

    EtherealSpear = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_ETHEREALSPEAR,
        blockedByArmor = true,
    },

    FierceBerserk = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        blockedByArmor = true,
        useCharges     = true,
        area           = createCombatArea(AREA_SQUARE1X1),
    },

    FireWave = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_HITBYFIRE,
        area         = createCombatArea(AREA_WAVE4, AREADIAGONAL_WAVE4),
    },

    FlameStrike = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
    },

    FrontSweep = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        blockedByArmor = true,
        useCharges     = true,
        area           = createCombatArea(AREA_WAVE6, AREADIAGONAL_WAVE6),
    },

    GreatEnergyBeam = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_ENERGYAREA,
        area         = createCombatArea(AREA_BEAM8),
    },

    Groundshaker = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_GROUNDSHAKER,
        blockedByArmor = true,
        useCharges     = true,
        area           = createCombatArea(AREA_CIRCLE3X3),
    },

    HellsCore = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_FIREAREA,
        area         = createCombatArea(AREA_CIRCLE5X5),
    },

    HolyFlash = {
        damageType     = Combat.DamageType.Holy,
        impactEffect   = CONST_ME_HOLYDAMAGE,
        distanceEffect = CONST_ANI_SMALLHOLY,
    },

    IceStrike = {
        damageType     = Combat.DamageType.Ice,
        impactEffect   = CONST_ME_ICEATTACK,
        distanceEffect = CONST_ANI_SMALLICE,
    },

    IceWave = {
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_ICEAREA,
        area         = createCombatArea(AREA_WAVE4, AREADIAGONAL_WAVE4),
    },

    Ignite = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_HITBYFIRE,
        distanceEffect = CONST_ANI_FIRE,
    },

    InflictWound = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_DRAWBLOOD,
        distanceEffect = CONST_ANI_WEAPONTYPE,
    },

    Lightning = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
    },

    MudAttack = {
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_SMALLEARTH,
    },

    PhysicalStrike = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_EXPLOSIONAREA,
        distanceEffect = CONST_ANI_EXPLOSION,
        blockedByArmor = true,
    },

    PractiseFireWave = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_HITBYFIRE,
        area         = createCombatArea(AREA_WAVE4, AREADIAGONAL_WAVE4),
    },

    RageOfTheSkies = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_BIGCLOUDS,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    Scorch = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_HITBYFIRE,
        area         = createCombatArea(AREA_WAVE4),
    },

    StrongEnergyStrike = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
    },

    StrongEtherealSpear = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_ETHEREALSPEAR,
        blockedByArmor = true,
    },

    StrongFlameStrike = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
    },

    StrongIceStrike = {
        damageType     = Combat.DamageType.Ice,
        impactEffect   = CONST_ME_ICEATTACK,
        distanceEffect = CONST_ANI_SMALLICE,
    },

    StrongIceWave = {
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_ICEAREA,
        area         = createCombatArea(AREA_WAVE3),
    },

    StrongTerraStrike = {
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_SMALLEARTH,
    },

    TerraStrike = {
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_SMALLEARTH,
    },

    TerraWave = {
        damageType   = Combat.DamageType.Earth,
        impactEffect = CONST_ME_SMALLPLANTS,
        area         = createCombatArea(AREA_SQUAREWAVE5, AREADIAGONAL_SQUAREWAVE5),
    },

    UltimateEnergyStrike = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
    },

    UltimateFlameStrike = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
    },

    UltimateIceStrike = {
        damageType     = Combat.DamageType.Ice,
        impactEffect   = CONST_ME_ICEATTACK,
        distanceEffect = CONST_ANI_SMALLICE,
    },

    UltimateTerraStrike = {
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_SMALLEARTH,
    },

    WhirlwindThrow = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_WEAPONTYPE,
        blockedByArmor = true,
        useCharges     = true,
    },

    WrathOfNature = {
        damageType   = Combat.DamageType.Earth,
        impactEffect = CONST_ME_SMALLPLANTS,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },
}
