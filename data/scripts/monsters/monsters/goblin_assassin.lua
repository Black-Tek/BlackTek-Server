local mtype = Game.createMonsterType("Goblin Assassin")
local monster = {}

monster.name = "Goblin Assassin"
monster.description = "a goblin assassin"
monster.experience = 52
monster.race = "blood"
monster.maxHealth = 75
monster.health = 75
monster.speed = 140
monster.manaCost = 360
monster.corpse = 6002
monster.outfit = { lookType = 296 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 10000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -15,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        duration = 10000,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 6,
        minDamage = 0,
        maxDamage = -35,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 160,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Goblin Powahhh!", yell = false},
    {text = "Me kill you!", yell = false},
    {text = "Me green menace!", yell = false},
    {text = "Gobabunga!", yell = false},
    {text = "Gooobliiiins!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 49810,
        maxCount = 9,
    },
    {
        id = 2379,
        chance = 17664,
    },
    {
        id = 2667,
        chance = 12472,
    },
    {
        id = 2230,
        chance = 12282,
    },
    {
        id = 1294,
        chance = 10620,
        maxCount = 3,
    },
    {
        id = 2461,
        chance = 10130,
    },
    {
        id = 2559,
        chance = 9687,
    },
    {
        id = 2406,
        chance = 8816,
    },
    {
        id = 2467,
        chance = 7281,
    },
    {
        id = 2235,
        chance = 6933,
    },
    {
        id = 2449,
        chance = 5065,
    },
}

mtype:register(monster)
