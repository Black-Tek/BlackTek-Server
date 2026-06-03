-- Combat table definitions for player healing spells.
-- Conditions that belong to the combat object are pre-created and embedded here.
-- Formula callbacks (setCallback) remain in individual spell files.

local _recoveryCondition = Condition(CONDITION_REGENERATION)
_recoveryCondition:setParameter(CONDITION_PARAM_TICKS, 60000)
_recoveryCondition:setParameter(CONDITION_PARAM_HEALTHGAIN, 20)
_recoveryCondition:setParameter(CONDITION_PARAM_HEALTHTICKS, 3000)
_recoveryCondition:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

local _intenseRecoveryCondition = Condition(CONDITION_REGENERATION)
_intenseRecoveryCondition:setParameter(CONDITION_PARAM_TICKS, 60000)
_intenseRecoveryCondition:setParameter(CONDITION_PARAM_HEALTHGAIN, 40)
_intenseRecoveryCondition:setParameter(CONDITION_PARAM_HEALTHTICKS, 3000)
_intenseRecoveryCondition:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

HealingCombats = {

    -- Cure spells (no damage type — just clear a debuff via execute)
    CureBleeding = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    CureBurning = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    CureCurse = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    CureElectrification = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    CurePoison = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    -- Regen spells (apply condition via combat)
    Recovery = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        condition    = _recoveryCondition,
    },

    IntenseRecovery = {
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
        condition    = _intenseRecoveryCondition,
    },

    -- Direct heal spells (formula callbacks stay in spell files)
    BruiseBane = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    DivineHealing = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    FairWoundCleansing = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    HealFriend = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
    },

    IntenseHealing = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    IntenseWoundCleansing = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    LightHealing = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    MagicPatch = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    -- Mass healing uses getTargets() manually in onCastSpell
    MassHealing = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    NaturesEmbrace = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_GREEN,
        aggressive   = false,
    },

    PractiseHealing = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    Restoration = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    Salvation = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    UltimateHealing = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },

    WoundCleansing = {
        damageType   = Combat.DamageType.Healing,
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
    },
}
