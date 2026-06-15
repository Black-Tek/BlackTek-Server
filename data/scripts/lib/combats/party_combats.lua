-- Combat table definitions and conditions for party spells.
-- The combat objects only carry area + aggressive; conditions are passed separately
-- to creature:addPartyCondition(combat, variant, PartyCombats.XxxCondition, baseMana).

local _enchantPartyCondition = Condition(CONDITION_ATTRIBUTES)
_enchantPartyCondition:setParameter(CONDITION_PARAM_TICKS, 2 * 60 * 1000)
_enchantPartyCondition:setParameter(CONDITION_PARAM_STAT_MAGICPOINTS, 1)
_enchantPartyCondition:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

local _healPartyCondition = Condition(CONDITION_REGENERATION)
_healPartyCondition:setParameter(CONDITION_PARAM_TICKS, 2 * 60 * 1000)
_healPartyCondition:setParameter(CONDITION_PARAM_HEALTHGAIN, 20)
_healPartyCondition:setParameter(CONDITION_PARAM_HEALTHTICKS, 2000)
_healPartyCondition:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

local _protectPartyCondition = Condition(CONDITION_ATTRIBUTES)
_protectPartyCondition:setParameter(CONDITION_PARAM_TICKS, 2 * 60 * 1000)
_protectPartyCondition:setParameter(CONDITION_PARAM_SKILL_SHIELD, 3)
_protectPartyCondition:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

local _trainPartyCondition = Condition(CONDITION_ATTRIBUTES)
_trainPartyCondition:setParameter(CONDITION_PARAM_TICKS, 2 * 60 * 1000)
_trainPartyCondition:setParameter(CONDITION_PARAM_SKILL_FIST, 3)
_trainPartyCondition:setParameter(CONDITION_PARAM_SKILL_MELEE, 3)
_trainPartyCondition:setParameter(CONDITION_PARAM_SKILL_DISTANCE, 3)
_trainPartyCondition:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

PartyCombats = {

    EnchantParty = {
        aggressive = false,
        area       = createCombatArea(AREA_CIRCLE3X3),
    },

    HealParty = {
        aggressive = false,
        area       = createCombatArea(AREA_CIRCLE3X3),
    },

    ProtectParty = {
        aggressive = false,
        area       = createCombatArea(AREA_CIRCLE3X3),
    },

    TrainParty = {
        aggressive = false,
        area       = createCombatArea(AREA_CIRCLE3X3),
    },
}

-- Conditions passed to addPartyCondition — exposed alongside combat tables
PartyCombats.EnchantPartyCondition = _enchantPartyCondition
PartyCombats.HealPartyCondition    = _healPartyCondition
PartyCombats.ProtectPartyCondition = _protectPartyCondition
PartyCombats.TrainPartyCondition   = _trainPartyCondition
