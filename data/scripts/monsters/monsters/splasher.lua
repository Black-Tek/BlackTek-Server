local mtype = Game.createMonsterType("Splasher")
local monster = {}

monster.name = "Splasher"
monster.description = "Splasher"
monster.experience = 500
monster.race = "blood"
monster.maxHealth = 1700
monster.health = 1700
monster.speed = 520
monster.manaCost = 0
monster.corpse = 6064
monster.outfit = { lookType = 72 }
monster.runHealth = 40
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1000,
        minDamage = 0,
        maxDamage = -382,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 8,
        range = 7,
        minDamage = -162,
        maxDamage = -228,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 7,
        minDamage = -106,
        maxDamage = -169,
        length = 8,
        spread = 3,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 9,
        minDamage = -134,
        maxDamage = -148,
        length = 8,
        effect = CONST_ME_BUBBLES,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 12,
        minDamage = -101,
        maxDamage = -149,
        radius = 3,
        target = false,
        effect = CONST_ME_BUBBLES,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 100,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -25},
    {type = COMBAT_EARTHDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Qua hah tsh!", yell = false},
    {text = "Teech tsha tshul!", yell = false},
    {text = "Quara tsha Fach!", yell = false},
    {text = "Tssssha Quara!", yell = false},
    {text = "Blubber.", yell = false},
    {text = "Blup.", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 13000,
        maxCount = 4,
    },
}

mtype:register(monster)
