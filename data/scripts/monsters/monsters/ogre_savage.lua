local mtype = Game.createMonsterType("Ogre Savage")
local monster = {}

monster.name = "Ogre Savage"
monster.description = "an ogre savage"
monster.experience = 625
monster.race = "blood"
monster.maxHealth = 800
monster.health = 800
monster.speed = 105
monster.manaCost = 0
monster.corpse = 24803
monster.outfit = { lookType = 858 }
monster.changeTarget = {
    interval = 2000,
    chance = 5,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        attack = 60,
        skill = 70,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -140,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -60,
        maxDamage = -200,
        radius = 5,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -20,
        maxDamage = -25,
        radius = 4,
        effect = CONST_ME_DRAWBLOOD,
    },
}
monster.defenses = {
    defense = 24,
    armor = 30,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 210,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "poison", combat = true, condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "You tasty!", yell = false},
    {text = "Must! Chop! Food! Raahh!", yell = false},
    {text = "UGGA UGGA!!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 54,
    },
    {
        id = "ogre ear stud",
        chance = 19720,
    },
    {
        id = "ogre nose ring",
        chance = 17360,
    },
    {
        id = "strong health potion",
        chance = 15330,
    },
    {
        id = "skull fetish",
        chance = 9300,
    },
    {
        id = "cookie",
        chance = 8350,
        maxCount = 5,
    },
    {
        id = "mysterious fetish",
        chance = 6120,
    },
    {
        id = "jalapeno pepper",
        chance = 4130,
        maxCount = 2,
    },
    {
        id = "onyx chip",
        chance = 3060,
        maxCount = 2,
    },
    {
        id = "small ruby",
        chance = 2840,
        maxCount = 2,
    },
    {
        id = "small topaz",
        chance = 2500,
        maxCount = 2,
    },
    {
        id = "opal",
        chance = 2240,
        maxCount = 2,
    },
    {
        id = "war hammer",
        chance = 1510,
    },
    {
        id = "ogre choppa",
        chance = 1160,
    },
    {
        id = "berserk potion",
        chance = 520,
    },
    {
        id = "shamanic mask",
        chance = 340,
    },
    {
        id = "dreaded cleaver",
        chance = 40,
    },
}

mtype:register(monster)
