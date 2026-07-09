local mtype = Game.createMonsterType("Prince Drazzak")
local monster = {}

monster.name = "Prince Drazzak"
monster.description = "Prince Drazzak"
monster.experience = 210000
monster.race = "fire"
monster.maxHealth = 330000
monster.health = 330000
monster.speed = 480
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 77,
    lookBody = 78,
    lookLegs = 94,
    lookFeet = 97,
}
monster.runHealth = 2000
monster.changeTarget = {
    interval = 2000,
    chance = 25,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        attack = 100,
        skill = 190,
    },
    {
        name = "death",
        interval = 2000,
        chance = 18,
        minDamage = -1000,
        maxDamage = -3000,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 65,
    armor = 55,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 1500,
        maxDamage = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "EVEN WITH ALL THAT TIME IN THE PRISON THAT WEAKENED ME, YOU ARE NO MATCH TO ME!", yell = true},
    {text = "DIE!", yell = true},
    {text = "SORCERERS MUST DIE!", yell = true},
    {text = "DRUIDS MUST DIE!", yell = true},
    {text = "PALADINS MUST DIE!", yell = true},
    {text = "KNIGHTS MUST DIE!", yell = true},
    {text = "GET OVER HERE!", yell = true},
    {text = "CRUSH THEM ALL!", yell = true},
    {text = "VARIPHOR WILL RULE!", yell = true},
    {text = "THEY WILL ALL PAY!", yell = true},
    {text = "NOT EVEN AEONS OF IMPRISONMENT WILL STOP ME!", yell = true},
    {text = "They used you fools to escape and they left ME behind!!??", yell = false},
}
monster.loot = {
    {
        id = 22396,
        chance = 100000,
        maxCount = 5,
    },
    {
        id = 22598,
        chance = 1000,
        maxCount = 3,
    },
    {
        id = 6500,
        chance = 100000,
        maxCount = 2,
    },
    {
        id = 5954,
        chance = 50000,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 50,
    },
    {
        id = 8473,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 8472,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 7590,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 22613,
        chance = 25000,
    },
    {
        id = 22608,
        chance = 2500,
    },
    {
        id = 22611,
        chance = 25000,
    },
    {
        id = 22612,
        chance = 25000,
    },
    {
        id = 5741,
        chance = 2500,
    },
    {
        id = 7417,
        chance = 2500,
    },
    {
        id = 22610,
        chance = 7000,
    },
    {
        id = 7418,
        chance = 1000,
    },
    {
        id = 7893,
        chance = 1000,
    },
    {
        id = 7632,
        chance = 5000,
    },
    {
        id = 7633,
        chance = 5000,
    },
}

mtype:register(monster)
