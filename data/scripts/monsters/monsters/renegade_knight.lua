local mtype = Game.createMonsterType("Renegade Knight")
local monster = {}

monster.name = "Renegade Knight"
monster.description = "a renegade knight"
monster.experience = 1200
monster.race = "blood"
monster.maxHealth = 1450
monster.health = 1450
monster.speed = 210
monster.manaCost = 0
monster.corpse = 20418
monster.outfit = {
    lookType = 268,
    lookHead = 97,
    lookBody = 113,
    lookLegs = 76,
    lookFeet = 98,
    lookAddons = 2,
}
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
        attack = 70,
        skill = 60,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = -25,
        maxDamage = -125,
        radius = 3,
        effect = CONST_ME_BLOCKHIT,
    },
}
monster.defenses = {
    defense = 33,
    armor = 27,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 35},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "I'll teach you a lesson!", yell = false},
    {text = "Feel my steel!", yell = false},
    {text = "Take this!", yell = false},
    {text = "Your abilities are no match for mine!", yell = false},
    {text = "Let's see how good you are!", yell = false},
    {text = "A challenge at last!", yell = false},
}
monster.loot = {
    {
        id = 12406,
        chance = 1655,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2152,
        chance = 25165,
        maxCount = 2,
    },
    {
        id = 12466,
        chance = 4635,
    },
    {
        id = 2666,
        chance = 7615,
    },
    {
        id = 2377,
        chance = 5960,
    },
    {
        id = 2391,
        chance = 993,
    },
    {
        id = 5911,
        chance = 2317,
    },
    {
        id = 2487,
        chance = 662,
    },
    {
        id = 2491,
        chance = 331,
    },
    {
        id = 7591,
        chance = 7284,
    },
    {
        id = 2147,
        chance = 6622,
    },
    {
        id = 2114,
        chance = 331,
    },
    {
        id = 2488,
        chance = 331,
    },
}

mtype:register(monster)
