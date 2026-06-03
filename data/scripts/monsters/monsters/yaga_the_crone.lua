local mtype = Game.createMonsterType("Yaga The Crone")
local monster = {}

monster.name = "Yaga The Crone"
monster.description = "Yaga the Crone"
monster.experience = 375
monster.race = "blood"
monster.maxHealth = 620
monster.health = 620
monster.speed = 240
monster.manaCost = 0
monster.corpse = 6081
monster.outfit = { lookType = 54 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
    {
        name = "fire",
        interval = 2500,
        chance = 50,
        range = 7,
        minDamage = -30,
        maxDamage = -50,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "poisoncondition",
        interval = 3000,
        chance = 13,
        range = 7,
        minDamage = -200,
        maxDamage = -300,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 13,
        range = 7,
        shootEffect = CONST_ANI_FIRE,
    },
}
monster.defenses = {
    defense = 20,
    armor = 15,
    {
        name = "invisible",
        interval = 1000,
        chance = 9,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "outfit",
        interval = 4000,
        chance = 9,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = 1},
    {type = COMBAT_PHYSICALDAMAGE, percent = -1},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Where did I park my hut?", yell = false},
    {text = "You will taste so sweet!", yell = false},
    {text = "Hexipooh, bewitched are you!", yell = false},
}
monster.loot = {
    {
        id = 2654,
        chance = 68000,
    },
    {
        id = 2324,
        chance = 64000,
    },
    {
        id = 2687,
        chance = 64000,
        maxCount = 8,
    },
    {
        id = 2148,
        chance = 32000,
        maxCount = 74,
    },
    {
        id = 2129,
        chance = 24000,
    },
    {
        id = 2800,
        chance = 20000,
    },
    {
        id = 2651,
        chance = 8000,
    },
    {
        id = 2199,
        chance = 8000,
    },
    {
        id = 8902,
        chance = 8000,
    },
    {
        id = 2185,
        chance = 4000,
    },
    {
        id = 2402,
        chance = 4000,
    },
}

mtype:register(monster)
