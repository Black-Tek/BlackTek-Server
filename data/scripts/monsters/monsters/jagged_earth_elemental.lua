local mtype = Game.createMonsterType("Jagged Earth Elemental")
local monster = {}

monster.name = "Jagged Earth Elemental"
monster.description = "a jagged earth elemental"
monster.experience = 1300
monster.race = "undead"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 280
monster.manaCost = 0
monster.corpse = 8933
monster.outfit = { lookType = 285 }
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
        maxDamage = -300,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 10,
        minDamage = -100,
        maxDamage = -250,
        length = 6,
        effect = CONST_ME_STONES,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 11,
        range = 7,
        minDamage = 0,
        maxDamage = -200,
        radius = 6,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "*STOMP STOMP*", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 87298,
        maxCount = 175,
    },
    {
        id = 11222,
        chance = 48625,
    },
    {
        id = 1294,
        chance = 24919,
        maxCount = 10,
    },
    {
        id = 2245,
        chance = 20874,
    },
    {
        id = 8298,
        chance = 7686,
    },
    {
        id = 2149,
        chance = 5502,
        maxCount = 2,
    },
    {
        id = 7732,
        chance = 2184,
    },
    {
        id = 11339,
        chance = 1133,
    },
    {
        id = 13757,
        chance = 485,
    },
    {
        id = 5880,
        chance = 324,
    },
}

mtype:register(monster)
