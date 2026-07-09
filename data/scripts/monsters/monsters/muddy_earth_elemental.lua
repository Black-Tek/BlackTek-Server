local mtype = Game.createMonsterType("Muddy Earth Elemental")
local monster = {}

monster.name = "Muddy Earth Elemental"
monster.description = "a muddy earth elemental"
monster.experience = 450
monster.race = "undead"
monster.maxHealth = 650
monster.health = 650
monster.speed = 260
monster.manaCost = 0
monster.corpse = 8933
monster.outfit = { lookType = 301 }
monster.changeTarget = {
    interval = 20000,
    chance = 50,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -160,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -25,
        maxDamage = -155,
        radius = 2,
        target = true,
        effect = CONST_ME_STONES,
    },
    {
        name = "poisoncondition",
        interval = 1000,
        chance = 10,
        minDamage = 0,
        maxDamage = -26,
        length = 6,
        effect = CONST_ME_GROUNDSHAKER,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.loot = {
    {
        id = "small stone",
        chance = 40000,
        maxCount = 3,
    },
    {
        id = "gold coin",
        chance = 24500,
        maxCount = 80,
    },
    {
        id = "gold coin",
        chance = 24500,
        maxCount = 47,
    },
    {
        id = 2244,
        chance = 22000,
    },
    {
        id = "natural soil",
        chance = 3750,
    },
}

mtype:register(monster)
