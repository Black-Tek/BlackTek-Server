local mtype = Game.createMonsterType("Ogre Brute")
local monster = {}

monster.name = "Ogre Brute"
monster.description = "an ogre brute"
monster.experience = 800
monster.race = "blood"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 102
monster.manaCost = 0
monster.corpse = 24799
monster.outfit = { lookType = 857 }
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
        skill = 60,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        shootEffect = CONST_ANI_LARGEROCK,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -60,
        maxDamage = -90,
        radius = 3,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 1,
        radius = 4,
        duration = 4000,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 4,
        radius = 3,
        target = true,
        speed = -400,
        shootEffect = CONST_ANI_LARGEROCK,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 24,
    armor = 41,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "poison", combat = true, condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "You so juicy!", yell = false},
    {text = "Smash you face in!!!", yell = false},
    {text = "You stop! You lunch!", yell = false},
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
        maxCount = 25,
    },
    {
        id = "ogre ear stud",
        chance = 18740,
    },
    {
        id = "meat",
        chance = 14920,
        maxCount = 2,
    },
    {
        id = "ogre nose ring",
        chance = 14660,
    },
    {
        id = "strong health potion",
        chance = 12580,
    },
    {
        id = "battle stone",
        chance = 4980,
    },
    {
        id = "small stone",
        chance = 4980,
        maxCount = 5,
    },
    {
        id = "cookie",
        chance = 3900,
        maxCount = 5,
    },
    {
        id = "skull fetish",
        chance = 3350,
    },
    {
        id = "small ruby",
        chance = 2460,
        maxCount = 2,
    },
    {
        id = "club ring",
        chance = 2340,
    },
    {
        id = "white pearl",
        chance = 2240,
    },
    {
        id = "onyx chip",
        chance = 2150,
        maxCount = 2,
    },
    {
        id = "power ring",
        chance = 1630,
    },
    {
        id = "opal",
        chance = 1560,
        maxCount = 2,
    },
    {
        id = "ogre klubba",
        chance = 1040,
    },
    {
        id = "pot",
        chance = 310,
    },
    {
        id = 9821,
        chance = 260,
    },
    {
        id = "ancient stone",
        chance = 1900,
    },
    {
        id = "bonebreaker",
        chance = 140,
    },
    {
        id = "shamanic mask",
        chance = 140,
    },
    {
        id = "butcher's axe",
        chance = 20,
    },
}

mtype:register(monster)
