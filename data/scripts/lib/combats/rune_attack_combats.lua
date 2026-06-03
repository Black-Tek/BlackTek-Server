-- Combat table definitions for attack rune spells.
-- Conditions embedded in the table are cloned per Combat() construction.
-- Formula callbacks (setCallback) remain in individual spell files.

local _paralyzeCondition = Condition(CONDITION_PARALYZE)
_paralyzeCondition:setParameter(CONDITION_PARAM_TICKS, 20000)
_paralyzeCondition:setFormula(-1, 80, -1, 80)

RuneAttackCombats = {

    -- Field / Wall creators (no damage type needed — just place an item)
    MagicWallRune = {
        distanceEffect = CONST_ANI_ENERGY,
        createdItem    = ITEM_MAGICWALL,
    },

    WildGrowthRune = {
        distanceEffect = CONST_ANI_ENERGY,
        createdItem    = ITEM_WILDGROWTH,
    },

    EnergyBombRune = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGYBALL,
        createdItem    = ITEM_ENERGYFIELD_PVP,
        area           = createCombatArea(AREA_SQUARE1X1),
    },

    EnergyFieldRune = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGYBALL,
        createdItem    = ITEM_ENERGYFIELD_PVP,
    },

    EnergyWallRune = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGYBALL,
        createdItem    = ITEM_ENERGYFIELD_PVP,
        area           = createCombatArea(AREA_WALLFIELD, AREADIAGONAL_WALLFIELD),
    },

    FireBombRune = {
        damageType     = Combat.DamageType.Fire,
        distanceEffect = CONST_ANI_FIRE,
        createdItem    = ITEM_FIREFIELD_PVP_FULL,
        area           = createCombatArea(AREA_SQUARE1X1),
    },

    FireFieldRune = {
        damageType     = Combat.DamageType.Fire,
        distanceEffect = CONST_ANI_FIRE,
        createdItem    = ITEM_FIREFIELD_PVP_FULL,
    },

    FireWallRune = {
        damageType     = Combat.DamageType.Fire,
        distanceEffect = CONST_ANI_FIRE,
        createdItem    = ITEM_FIREFIELD_PVP_FULL,
        area           = createCombatArea(AREA_WALLFIELD, AREADIAGONAL_WALLFIELD),
    },

    PoisonBombRune = {
        damageType     = Combat.DamageType.Earth,
        distanceEffect = CONST_ANI_ENERGY,
        createdItem    = ITEM_POISONFIELD_PVP,
        area           = createCombatArea(AREA_SQUARE1X1),
    },

    PoisonFieldRune = {
        damageType     = Combat.DamageType.Earth,
        distanceEffect = CONST_ANI_ENERGY,
        createdItem    = ITEM_POISONFIELD_PVP,
    },

    PoisonWallRune = {
        damageType     = Combat.DamageType.Earth,
        distanceEffect = CONST_ANI_ENERGY,
        createdItem    = ITEM_POISONFIELD_PVP,
        area           = createCombatArea(AREA_WALLFIELD, AREADIAGONAL_WALLFIELD),
    },

    -- Condition rune
    ParalyzeRune = {
        impactEffect = CONST_ME_MAGIC_RED,
        condition    = _paralyzeCondition,
    },

    -- Area-damage runes (formula callbacks stay in spell files)
    AvalancheRune = {
        damageType     = Combat.DamageType.Ice,
        impactEffect   = CONST_ME_ICEAREA,
        distanceEffect = CONST_ANI_ICE,
        area           = createCombatArea(AREA_CIRCLE3X3),
    },

    ExplosionRune = {
        damageType     = Combat.DamageType.Physical,
        impactEffect   = CONST_ME_EXPLOSIONAREA,
        distanceEffect = CONST_ANI_EXPLOSION,
        blockedByArmor = true,
        area           = createCombatArea(AREA_CROSS1X1),
    },

    GreatFireballRune = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREAREA,
        distanceEffect = CONST_ANI_FIRE,
        area           = createCombatArea(AREA_CIRCLE3X3),
    },

    LightStoneShowerRune = {
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_STONES,
        distanceEffect = CONST_ANI_EARTH,
        area           = createCombatArea(AREA_CROSS1X1),
    },

    StoneShowerRune = {
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_STONES,
        distanceEffect = CONST_ANI_EARTH,
        area           = createCombatArea(AREA_CIRCLE3X3),
    },

    ThunderstormRune = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGYBALL,
        area           = createCombatArea(AREA_CIRCLE3X3),
    },

    -- Single-target runes (formula callbacks stay in spell files)
    FireballRune = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREATTACK,
        distanceEffect = CONST_ANI_FIRE,
    },

    HeavyMagicMissileRune = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGY,
    },

    HolyMissileRune = {
        damageType     = Combat.DamageType.Holy,
        impactEffect   = CONST_ME_HOLYAREA,
        distanceEffect = CONST_ANI_HOLY,
    },

    IcicleRune = {
        damageType     = Combat.DamageType.Ice,
        impactEffect   = CONST_ME_ICEATTACK,
        distanceEffect = CONST_ANI_SMALLICE,
    },

    LightestMagicMissileRune = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYAREA,
        distanceEffect = CONST_ANI_ENERGY,
    },

    LightestMissileRune = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGY,
    },

    LightMagicMissileRune = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGY,
    },

    SoulfireRune = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_HITBYFIRE,
        distanceEffect = CONST_ANI_FIRE,
    },

    StalagmiteRune = {
        damageType     = Combat.DamageType.Earth,
        impactEffect   = CONST_ME_STONES,
        distanceEffect = CONST_ANI_EARTH,
    },

    SuddenDeathRune = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_MORTAREA,
        distanceEffect = CONST_ANI_SUDDENDEATH,
    },
}
