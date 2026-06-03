-- Combat table definitions for healing rune spells.
-- Formula callbacks (setCallback) remain in individual spell files.

RuneHealingCombats = {

    -- Cure Poison Rune applies no damage — just a visual effect on execute
    CurePoisonRune = {
        impactEffect  = CONST_ME_MAGIC_BLUE,
        topTargetOnly = true,
        aggressive    = false,
    },

    IntenseHealingRune = {
        damageType    = Combat.DamageType.Healing,
        impactEffect  = CONST_ME_MAGIC_BLUE,
        topTargetOnly = true,
        aggressive    = false,
    },

    UltimateHealingRune = {
        damageType    = Combat.DamageType.Healing,
        impactEffect  = CONST_ME_MAGIC_BLUE,
        topTargetOnly = true,
        aggressive    = false,
    },
}
