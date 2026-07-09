local mtype = Game.createMonsterType("Zulazza the Corruptor")
local monster = {}

monster.name = "Zulazza the Corruptor"
monster.description = "Zulazza The Corruptor"
monster.experience = 10000
monster.race = "blood"
monster.maxHealth = 46500
monster.health = 46500
monster.speed = 290
monster.manaCost = 0
monster.corpse = 11107
monster.outfit = { lookType = 334 }
monster.runHealth = 1500
monster.changeTarget = {
    interval = 2000,
    chance = 10,
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
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -2100,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 40,
        minDamage = -500,
        maxDamage = -800,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 30,
        minDamage = -300,
        maxDamage = -800,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -50,
        maxDamage = -130,
        target = true,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        speed = -500,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 119,
    armor = 96,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 2000,
        maxDamage = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 70},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "energy", combat = true, condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 10,
    {text = "I'm Zulazza, and you won't forget me that fazzt.", yell = false},
    {text = "Oh, HE will take revenge on zzizz azzault when you zztep in front of HIZZ fazze!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 98,
    },
    {
        id = 2152,
        chance = 71429,
        maxCount = 6,
    },
    {
        id = 2155,
        chance = 42857,
    },
    {
        id = 2154,
        chance = 42857,
    },
    {
        id = 2127,
        chance = 28571,
    },
    {
        id = 7591,
        chance = 28571,
        maxCount = 4,
    },
    {
        id = 8472,
        chance = 28571,
        maxCount = 7,
    },
    {
        id = 2156,
        chance = 28571,
    },
    {
        id = 5944,
        chance = 28571,
    },
    {
        id = 2158,
        chance = 14286,
    },
    {
        id = "paladin armor",
        chance = 5500,
    },
    {
        id = "earthborn titan armor",
        chance = 550,
    },
    {
        id = 9810,
        chance = 14286,
    },
    {
        id = 11118,
        chance = 500,
    },
}

mtype:register(monster)
