local mtype = Game.createMonsterType("Minotaur Amazon")
local monster = {}

monster.name = "Minotaur Amazon"
monster.description = "a minotaur amazon"
monster.experience = 2200
monster.race = "blood"
monster.maxHealth = 2600
monster.health = 2600
monster.speed = 250
monster.manaCost = 0
monster.corpse = 23371
monster.outfit = { lookType = 608 }
monster.runHealth = 260
monster.changeTarget = {
    interval = 2000,
    chance = 11,
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -50,
        maxDamage = -305,
        length = 8,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 16,
        minDamage = -50,
        maxDamage = -150,
        radius = 4,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 22,
        range = 7,
        minDamage = 0,
        maxDamage = -150,
        shootEffect = CONST_ANI_HUNTINGSPEAR,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 40,
        minDamage = -300,
        maxDamage = -400,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_THROWINGKNIFE,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        radius = 1,
        target = true,
        speed = -600,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 37,
    armor = 37,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "I'll protect the herd!", yell = false},
    {text = "Never surrender!", yell = false},
    {text = "You won't hurt us!", yell = false},
    {text = "I will not allow your evil to continue!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 199,
    },
    {
        id = 2152,
        chance = 65302,
        maxCount = 3,
    },
    {
        id = 2671,
        chance = 59419,
    },
    {
        id = 23575,
        chance = 19739,
    },
    {
        id = 5878,
        chance = 16261,
    },
    {
        id = 7590,
        chance = 7272,
    },
    {
        id = 2666,
        chance = 6756,
    },
    {
        id = 7591,
        chance = 6670,
    },
    {
        id = 12428,
        chance = 5110,
        maxCount = 2,
    },
    {
        id = 9970,
        chance = 5082,
        maxCount = 2,
    },
    {
        id = 2147,
        chance = 5024,
        maxCount = 2,
    },
    {
        id = 2150,
        chance = 4924,
        maxCount = 2,
    },
    {
        id = 2149,
        chance = 4395,
        maxCount = 2,
    },
    {
        id = 23546,
        chance = 3851,
    },
    {
        id = 7368,
        chance = 3092,
        maxCount = 5,
    },
    {
        id = 2214,
        chance = 2376,
    },
    {
        id = 23545,
        chance = 1646,
    },
    {
        id = 2197,
        chance = 859,
    },
    {
        id = 7443,
        chance = 845,
    },
    {
        id = 2475,
        chance = 701,
    },
    {
        id = 9971,
        chance = 601,
    },
    {
        id = 5911,
        chance = 558,
    },
    {
        id = 2154,
        chance = 558,
    },
    {
        id = 2156,
        chance = 415,
    },
    {
        id = 7401,
        chance = 243,
    },
}

mtype:register(monster)
