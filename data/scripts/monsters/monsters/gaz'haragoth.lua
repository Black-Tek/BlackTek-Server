local mtype = Game.createMonsterType("Gaz'haragoth")
local monster = {}

monster.name = "Gaz'haragoth"
monster.description = "Gaz'haragoth"
monster.experience = 1000000
monster.race = "fire"
monster.maxHealth = 350000
monster.health = 350000
monster.speed = 400
monster.manaCost = 0
monster.corpse = 22562
monster.outfit = { lookType = 591 }
monster.changeTarget = {
    interval = 10000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 98
monster.flags = {
    attackable = true,
    hostile = true,
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
        maxDamage = -5000,
    },
    {
        name = "ice",
        interval = 3000,
        chance = 35,
        range = 7,
        minDamage = -900,
        maxDamage = -1100,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "melee",
        interval = 1800,
        chance = 40,
        range = 7,
        minDamage = 0,
        maxDamage = -1000,
        radius = 5,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 30,
        minDamage = -4000,
        maxDamage = -6000,
        length = 8,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "physical",
        interval = 2500,
        chance = 20,
        range = 7,
        minDamage = -200,
        maxDamage = -480,
        radius = 5,
        target = false,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 65,
    armor = 55,
    {
        name = "healing",
        interval = 3000,
        chance = 35,
        minDamage = 2500,
        maxDamage = 3500,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 4000,
        chance = 80,
        speed = 500,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 6
monster.summons = {
    {name = "Minion of Gaz'haragoth", interval = 4000, chance = 33, max = 3},
    {name = "Nightmare of Gaz'haragoth", interval = 4000, chance = 33, max = 3},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "NO ONE WILL ESCAPE ME!", yell = true},
    {text = "I'LL KEEP THE ORDER UP!", yell = true},
    {text = "You puny humans will be my snacks!", yell = true},
    {text = "I've beaten tougher demons than you even know!", yell = true},
    {text = "How dare you come to this place!", yell = false},
}
monster.loot = {
    {
        id = 22396,
        chance = 100000,
        maxCount = 13,
    },
    {
        id = 2160,
        chance = 100000,
        maxCount = 10,
    },
    {
        id = 22397,
        chance = 100000,
    },
    {
        id = 11258,
        chance = 100000,
    },
    {
        id = 11262,
        chance = 100000,
    },
    {
        id = 11260,
        chance = 100000,
    },
    {
        id = 11261,
        chance = 100000,
    },
    {
        id = 11259,
        chance = 100000,
    },
    {
        id = 6500,
        chance = 92857,
    },
    {
        id = 22598,
        chance = 92857,
        maxCount = 3,
    },
    {
        id = 5911,
        chance = 71429,
    },
    {
        id = 5914,
        chance = 71429,
    },
    {
        id = 18416,
        chance = 50000,
        maxCount = 23,
    },
    {
        id = 18413,
        chance = 35714,
        maxCount = 22,
    },
    {
        id = 8472,
        chance = 35714,
        maxCount = 183,
    },
    {
        id = 8473,
        chance = 35714,
        maxCount = 119,
    },
    {
        id = 2158,
        chance = 28571,
    },
    {
        id = 5954,
        chance = 28571,
    },
    {
        id = 7632,
        chance = 28571,
    },
    {
        id = 7590,
        chance = 28571,
        maxCount = 181,
    },
    {
        id = 18415,
        chance = 28571,
        maxCount = 20,
    },
    {
        id = 6529,
        chance = 28571,
        maxCount = 194,
    },
    {
        id = 18418,
        chance = 21429,
        maxCount = 27,
    },
    {
        id = 18417,
        chance = 21429,
        maxCount = 28,
    },
    {
        id = 22401,
        chance = 21429,
    },
    {
        id = 7368,
        chance = 14286,
        maxCount = 109,
    },
    {
        id = 22404,
        chance = 14286,
    },
    {
        id = 22398,
        chance = 14286,
    },
    {
        id = 22614,
        chance = 14286,
    },
    {
        id = 2195,
        chance = 7143,
    },
    {
        id = 22419,
        chance = 7143,
    },
    {
        id = 22413,
        chance = 7143,
    },
    {
        id = 22422,
        chance = 7143,
    },
    {
        id = 22610,
        chance = 7143,
    },
    {
        id = 22613,
        chance = 7143,
    },
    {
        id = 2168,
        chance = 7143,
    },
    {
        id = 7899,
        chance = 7143,
    },
    {
        id = 22616,
        chance = 7143,
    },
    {
        id = 22608,
        chance = 7143,
    },
    {
        id = 22611,
        chance = 7143,
    },
    {
        id = 22408,
        chance = 7143,
    },
    {
        id = 22400,
        chance = 7143,
    },
    {
        id = 18414,
        chance = 7143,
        maxCount = 26,
    },
}

mtype:register(monster)
