-- Combat table definitions for player support spells.
-- Conditions that belong to the combat object are pre-created and embedded here.
-- cancel_magic_shield is omitted — it is conditional on a config flag and keeps its combat inline.

local _bloodRageCondition = Condition(CONDITION_ATTRIBUTES)
_bloodRageCondition:setParameter(CONDITION_PARAM_TICKS, 10000)
_bloodRageCondition:setParameter(CONDITION_PARAM_SKILL_MELEEPERCENT, 135)
_bloodRageCondition:setParameter(CONDITION_PARAM_DISABLE_DEFENSE, true)
_bloodRageCondition:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

local _chargeCondition = Condition(CONDITION_HASTE)
_chargeCondition:setParameter(CONDITION_PARAM_TICKS, 5000)
_chargeCondition:setFormula(0.9, -72, 0.9, -72)

local _greatLightCondition = Condition(CONDITION_LIGHT)
_greatLightCondition:setParameter(CONDITION_PARAM_LIGHT_LEVEL, 8)
_greatLightCondition:setParameter(CONDITION_PARAM_LIGHT_COLOR, 215)
_greatLightCondition:setParameter(CONDITION_PARAM_TICKS, (11 * 60 + 35) * 1000)

local _hasteCondition = Condition(CONDITION_HASTE)
_hasteCondition:setParameter(CONDITION_PARAM_TICKS, 33000)
_hasteCondition:setFormula(0.3, -24, 0.3, -24)

local _invisibleCondition = Condition(CONDITION_INVISIBLE)
_invisibleCondition:setParameter(CONDITION_PARAM_TICKS, 200000)

local _lightCondition = Condition(CONDITION_LIGHT)
_lightCondition:setParameter(CONDITION_PARAM_LIGHT_LEVEL, 6)
_lightCondition:setParameter(CONDITION_PARAM_LIGHT_COLOR, 215)
_lightCondition:setParameter(CONDITION_PARAM_TICKS, (6 * 60 + 10) * 1000)

local _magicShieldCondition = Condition(CONDITION_MANASHIELD)
_magicShieldCondition:setParameter(CONDITION_PARAM_TICKS, 200000)

local _protectorSkill = Condition(CONDITION_ATTRIBUTES)
_protectorSkill:setParameter(CONDITION_PARAM_TICKS, 13000)
_protectorSkill:setParameter(CONDITION_PARAM_SKILL_SHIELDPERCENT, 220)
_protectorSkill:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

local _protectorCooldown = Condition(CONDITION_SPELLGROUPCOOLDOWN)
_protectorCooldown:setParameter(CONDITION_PARAM_SUBID, 1)
_protectorCooldown:setParameter(CONDITION_PARAM_TICKS, 10000)

local _protectorPacified = Condition(CONDITION_PACIFIED)
_protectorPacified:setParameter(CONDITION_PARAM_TICKS, 10000)

local _sharpshooterSkill = Condition(CONDITION_ATTRIBUTES)
_sharpshooterSkill:setParameter(CONDITION_PARAM_TICKS, 10000)
_sharpshooterSkill:setParameter(CONDITION_PARAM_SKILL_DISTANCEPERCENT, 140)
_sharpshooterSkill:setParameter(CONDITION_PARAM_DISABLE_DEFENSE, true)
_sharpshooterSkill:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

local _sharpshooterSpeed = Condition(CONDITION_PARALYZE)
_sharpshooterSpeed:setParameter(CONDITION_PARAM_TICKS, 10000)
_sharpshooterSpeed:setFormula(-0.7, 56, -0.7, 56)

local _strongHasteCondition = Condition(CONDITION_HASTE)
_strongHasteCondition:setParameter(CONDITION_PARAM_TICKS, 22000)
_strongHasteCondition:setFormula(0.7, -56, 0.7, -56)

local _swiftFootSpeed = Condition(CONDITION_HASTE)
_swiftFootSpeed:setParameter(CONDITION_PARAM_TICKS, 10000)
_swiftFootSpeed:setFormula(0.8, -64, 0.8, -64)

local _swiftFootCooldown = Condition(CONDITION_SPELLGROUPCOOLDOWN)
_swiftFootCooldown:setParameter(CONDITION_PARAM_SUBID, 1)
_swiftFootCooldown:setParameter(CONDITION_PARAM_TICKS, 10000)

local _swiftFootPacified = Condition(CONDITION_PACIFIED)
_swiftFootPacified:setParameter(CONDITION_PARAM_TICKS, 10000)

local _ultimateLightCondition = Condition(CONDITION_LIGHT)
_ultimateLightCondition:setParameter(CONDITION_PARAM_LIGHT_LEVEL, 9)
_ultimateLightCondition:setParameter(CONDITION_PARAM_LIGHT_COLOR, 215)
_ultimateLightCondition:setParameter(CONDITION_PARAM_TICKS, (60 * 33 + 10) * 1000)

SupportCombats = {

    BloodRage = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        condition    = _bloodRageCondition,
    },

    -- Challenge uses setCallback(TARGETCREATURE) — callback stays in spell file
    CancelInvisibility = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    Challenge = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        area         = createCombatArea(AREA_SQUARE1X1),
    },

    Charge = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        condition    = _chargeCondition,
    },

    GreatLight = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        condition    = _greatLightCondition,
    },

    Haste = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        condition    = _hasteCondition,
    },

    Invisible = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        condition    = _invisibleCondition,
    },

    Light = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        condition    = _lightCondition,
    },

    MagicShield = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        condition    = _magicShieldCondition,
    },

    Protector = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        conditions   = { _protectorSkill, _protectorCooldown, _protectorPacified },
    },

    Sharpshooter = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        conditions   = { _sharpshooterSkill, _sharpshooterSpeed },
    },

    StrongHaste = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        condition    = _strongHasteCondition,
    },

    SwiftFoot = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        conditions   = { _swiftFootSpeed, _swiftFootCooldown, _swiftFootPacified },
    },

    UltimateLight = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        condition    = _ultimateLightCondition,
    },
}
