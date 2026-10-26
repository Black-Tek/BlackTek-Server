-- Combat table definitions for player attack spells.
-- Pass an entry to Combat(table) in the spell file to create the combat object.
-- Output formulas are registered per-spell via FormulaNode.random + combat:registerFormula.

AttackCombats = {

    Annihilation = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_WEAPONTYPE,
        blockedByArmor = true,
        useCharges     = true,
    },

    ApprenticesStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    Berserk = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        blockedByArmor = true,
        useCharges     = true,
        area           = createCombatArea(AREA_SQUARE1X1),
    },

    BrutalStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_WEAPONTYPE,
        blockedByArmor = true,
        useCharges     = true,
    },

    Buzz = {
        aggressive     = true,
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    ChillOut = {
        aggressive   = true,
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_ICEAREA,
        area         = createCombatArea(AREA_WAVE4),
    },

    Curse = {
        aggressive     = true,
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_MORTAREA,
        distanceEffect = CONST_ANI_DEATH,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    DeathStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_MORTAREA,
        distanceEffect = CONST_ANI_DEATH,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    DivineCaldera = {
        aggressive   = true,
        damageType   = Combat.DamageType.Holy,
        impactEffect = CONST_ME_HOLYAREA,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    DivineMissile = {
        aggressive     = true,
        damageType     = Combat.DamageType.Holy,
        impactEffect   = CONST_ME_HOLYDAMAGE,
        distanceEffect = CONST_ANI_SMALLHOLY,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    Electrify = {
        aggressive     = true,
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    EnergyBeam = {
        aggressive   = true,
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_ENERGYHIT,
        area         = createCombatArea(AREA_BEAM5, AREADIAGONAL_BEAM5),
    },

    EnergyStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    EnergyWave = {
        aggressive     = true,
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_SQUAREWAVE5, AREADIAGONAL_SQUAREWAVE5),
    },

    Envenom = {
        aggressive     = true,
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_EARTH,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    EternalWinter = {
        aggressive   = true,
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_ICETORNADO,
        area         = createCombatArea(AREA_CIRCLE5X5),
    },

    EtherealSpear = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_ETHEREALSPEAR,
        blockedByArmor = true,
    },

    FierceBerserk = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        blockedByArmor = true,
        useCharges     = true,
        area           = createCombatArea(AREA_SQUARE1X1),
    },

    FireWave = {
        aggressive   = true,
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_HITBYFIRE,
        area         = createCombatArea(AREA_WAVE4, AREADIAGONAL_WAVE4),
    },

    FlameStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    FrontSweep = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        blockedByArmor = true,
        useCharges     = true,
        area           = createCombatArea(AREA_WAVE6, AREADIAGONAL_WAVE6),
    },

    GreatEnergyBeam = {
        aggressive   = true,
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_ENERGYAREA,
        area         = createCombatArea(AREA_BEAM8),
    },

    Groundshaker = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_GROUNDSHAKER,
        blockedByArmor = true,
        useCharges     = true,
        area           = createCombatArea(AREA_CIRCLE3X3),
    },

    HellsCore = {
        aggressive   = true,
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_FIREAREA,
        area         = createCombatArea(AREA_CIRCLE5X5),
    },

    HolyFlash = {
        aggressive     = true,
        damageType     = Combat.DamageType.Holy,
        impactEffect   = CONST_ME_HOLYDAMAGE,
        distanceEffect = CONST_ANI_SMALLHOLY,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    IceStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Ice,
        impactEffect   = CONST_ME_ICEATTACK,
        distanceEffect = CONST_ANI_SMALLICE,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    IceWave = {
        aggressive   = true,
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_ICEAREA,
        area         = createCombatArea(AREA_WAVE4, AREADIAGONAL_WAVE4),
    },

    Ignite = {
        aggressive     = true,
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_HITBYFIRE,
        distanceEffect = CONST_ANI_FIRE,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    InflictWound = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_DRAWBLOOD,
        distanceEffect = CONST_ANI_WEAPONTYPE,
    },

    Lightning = {
        aggressive     = true,
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    MudAttack = {
        aggressive     = true,
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_SMALLEARTH,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    PhysicalStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_EXPLOSIONAREA,
        distanceEffect = CONST_ANI_EXPLOSION,
        blockedByArmor = true,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    PractiseFireWave = {
        aggressive   = true,
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_HITBYFIRE,
        area         = createCombatArea(AREA_WAVE4, AREADIAGONAL_WAVE4),
    },

    RageOfTheSkies = {
        aggressive   = true,
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_BIGCLOUDS,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    Scorch = {
        aggressive   = true,
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_HITBYFIRE,
        area         = createCombatArea(AREA_WAVE4),
    },

    StrongEnergyStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    StrongEtherealSpear = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_ETHEREALSPEAR,
        blockedByArmor = true,
    },

    StrongFlameStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    StrongIceStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Ice,
        impactEffect   = CONST_ME_ICEATTACK,
        distanceEffect = CONST_ANI_SMALLICE,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    StrongIceWave = {
        aggressive   = true,
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_ICEAREA,
        area         = createCombatArea(AREA_WAVE3),
    },

    StrongTerraStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_SMALLEARTH,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    TerraStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_SMALLEARTH,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    TerraWave = {
        aggressive   = true,
        damageType   = Combat.DamageType.Earth,
        impactEffect = CONST_ME_SMALLPLANTS,
        area         = createCombatArea(AREA_SQUAREWAVE5, AREADIAGONAL_SQUAREWAVE5),
    },

    UltimateEnergyStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    UltimateFlameStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    UltimateIceStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Ice,
        impactEffect   = CONST_ME_ICEATTACK,
        distanceEffect = CONST_ANI_SMALLICE,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    UltimateTerraStrike = {
        aggressive     = true,
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_CARNIPHILA,
        distanceEffect = CONST_ANI_SMALLEARTH,
        area           = createCombatArea(AREA_SINGLE_TILE),
    },

    WhirlwindThrow = {
        aggressive     = true,
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_HITAREA,
        distanceEffect = CONST_ANI_WEAPONTYPE,
        blockedByArmor = true,
        useCharges     = true,
    },

    WrathOfNature = {
        aggressive   = true,
        damageType   = Combat.DamageType.Earth,
        impactEffect = CONST_ME_SMALLPLANTS,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },
}
