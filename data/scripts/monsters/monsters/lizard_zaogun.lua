local mtype = Game.createMonsterType("Lizard Zaogun")
local monster = {}

monster.name = "Lizard Zaogun"
monster.description = "a lizard zaogun"
monster.experience = 1700
monster.race = "blood"
monster.maxHealth = 2955
monster.health = 2955
monster.speed = 276
monster.manaCost = 0
monster.corpse = 11284
monster.outfit = { lookType = 343 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -349,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -220,
        maxDamage = -375,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 42,
    armor = 42,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 175,
        maxDamage = 275,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 45},
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 94142,
        maxCount = 268,
    },
    {
        id = 2152,
        chance = 49665,
        maxCount = 2,
    },
    {
        id = 11331,
        chance = 14974,
    },
    {
        id = 5876,
        chance = 14674,
    },
    {
        id = 5881,
        chance = 12054,
    },
    {
        id = 11330,
        chance = 8381,
    },
    {
        id = 7591,
        chance = 7211,
        maxCount = 3,
    },
    {
        id = 2149,
        chance = 4993,
        maxCount = 5,
    },
    {
        id = 11206,
        chance = 2087,
    },
    {
        id = 7588,
        chance = 1951,
    },
    {
        id = 11303,
        chance = 1118,
    },
    {
        id = 2528,
        chance = 1020,
    },
    {
        id = 11304,
        chance = 912,
    },
    {
        id = 11301,
        chance = 519,
    },
}

mtype:register(monster)
