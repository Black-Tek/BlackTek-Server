local mtype = Game.createMonsterType("Vicious Squire")
local monster = {}

monster.name = "Vicious Squire"
monster.description = "a vicious squire"
monster.experience = 900
monster.race = "blood"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 210
monster.manaCost = 0
monster.corpse = 20418
monster.outfit = {
    lookType = 131,
    lookHead = 97,
    lookBody = 24,
    lookLegs = 73,
    lookFeet = 116,
    lookAddons = 1,
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
        attack = 50,
        skill = 82,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -110,
        shootEffect = CONST_ANI_BOLT,
    },
}
monster.defenses = {
    defense = 33,
    armor = 27,
    {
        name = "healing",
        interval = 2000,
        chance = 7,
        minDamage = 150,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 40},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "For hurting me, my sire will kill you!", yell = false},
    {text = "Your stuff will be mine soon!", yell = false},
    {text = "I'll cut you a bloody grin!", yell = false},
    {text = "You shouldn't have come here!", yell = false},
    {text = "You are as good as dead!", yell = false},
}
monster.loot = {
    {
        id = 2455,
        chance = 12763,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 70,
    },
    {
        id = 2661,
        chance = 743,
    },
    {
        id = 2543,
        chance = 21685,
        maxCount = 10,
    },
    {
        id = 2666,
        chance = 11771,
    },
    {
        id = 2145,
        chance = 2602,
    },
    {
        id = 2149,
        chance = 3097,
    },
    {
        id = 2150,
        chance = 2973,
    },
    {
        id = 2391,
        chance = 991,
    },
    {
        id = 2381,
        chance = 2230,
    },
    {
        id = 2477,
        chance = 619,
    },
    {
        id = 2515,
        chance = 247,
    },
    {
        id = 2475,
        chance = 123,
    },
    {
        id = 7591,
        chance = 5576,
    },
    {
        id = 2164,
        chance = 247,
    },
}

mtype:register(monster)
