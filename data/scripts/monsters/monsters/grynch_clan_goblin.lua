local mtype = Game.createMonsterType("Grynch Clan Goblin")
local monster = {}

monster.name = "Grynch Clan Goblin"
monster.description = "Grynch Clan Goblin"
monster.experience = 4
monster.race = "blood"
monster.maxHealth = 80
monster.health = 80
monster.speed = 200
monster.manaCost = 0
monster.corpse = 6002
monster.outfit = { lookType = 61 }
monster.runHealth = 80
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 11
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
}
monster.defenses = {
    defense = 5,
    armor = 5,
    {
        name = "speed",
        interval = 1000,
        chance = 15,
        speed = 300,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "T'was not me hand in your pocket!", yell = false},
    {text = "Look! Cool stuff in house. Let's get it!", yell = false},
    {text = "Uhh! Nobody home! <chuckle>", yell = false},
    {text = "Me just borrowed it!", yell = false},
    {text = "Me no steal! Me found it!", yell = false},
    {text = "Me had it for five minutes. It's family heirloom now!", yell = false},
    {text = "Nice human won't hurt little, good goblin?", yell = false},
    {text = "Gimmegimme!", yell = false},
    {text = "Invite me in you lovely house plx!", yell = false},
    {text = "Other Goblin stole it!", yell = false},
    {text = "All presents mine!", yell = false},
    {text = "Me got ugly ones purse", yell = false},
    {text = "Free itans plz!", yell = false},
    {text = "Not me! Not me!", yell = false},
    {text = "Guys, help me here! Guys? Guys???", yell = false},
    {text = "That only much dust in me pocket! Honest!", yell = false},
    {text = "Can me have your stuff?", yell = false},
    {text = "Halp, Big dumb one is after me!", yell = false},
    {text = "Uh, So many shiny things!", yell = false},
    {text = "Utani hur hur hur!", yell = false},
    {text = "Mee? Stealing? Never!!!", yell = false},
    {text = "Oh what fun it is to steal a one-horse open sleigh!", yell = false},
    {text = "Must have it! Must have it!", yell = false},
    {text = "Where me put me lockpick?", yell = false},
    {text = "Catch me if you can!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 55320,
        maxCount = 16,
    },
    {
        id = 6497,
        chance = 29918,
    },
    {
        id = 2111,
        chance = 14919,
        maxCount = 3,
    },
    {
        id = 2674,
        chance = 10196,
        maxCount = 3,
    },
    {
        id = 2687,
        chance = 8023,
        maxCount = 5,
    },
    {
        id = 2675,
        chance = 5033,
        maxCount = 2,
    },
    {
        id = 6277,
        chance = 4934,
    },
    {
        id = 7909,
        chance = 1998,
        maxCount = 4,
    },
    {
        id = 3606,
        chance = 1019,
        maxCount = 2,
    },
    {
        id = 2688,
        chance = 997,
        maxCount = 3,
    },
    {
        id = 10207,
        chance = 965,
    },
    {
        id = 7910,
        chance = 961,
        maxCount = 5,
    },
    {
        id = 2661,
        chance = 808,
    },
    {
        id = 5894,
        chance = 638,
        maxCount = 3,
    },
    {
        id = 2007,
        chance = 543,
    },
    {
        id = 4873,
        chance = 476,
    },
    {
        id = 2324,
        chance = 400,
    },
    {
        id = 5902,
        chance = 400,
    },
    {
        id = 6501,
        chance = 391,
    },
    {
        id = 5890,
        chance = 382,
        maxCount = 3,
    },
    {
        id = 2679,
        chance = 341,
        maxCount = 4,
    },
    {
        id = 2260,
        chance = 121,
    },
    {
        id = 2983,
        chance = 117,
    },
    {
        id = 2036,
        chance = 117,
    },
    {
        id = 1852,
        chance = 112,
    },
    {
        id = 6393,
        chance = 108,
    },
    {
        id = 2162,
        chance = 108,
    },
    {
        id = 6393,
        chance = 108,
    },
    {
        id = 5792,
        chance = 103,
    },
    {
        id = 2159,
        chance = 99,
    },
    {
        id = 2072,
        chance = 90,
    },
    {
        id = 5022,
        chance = 81,
    },
    {
        id = 2120,
        chance = 81,
    },
    {
        id = 1684,
        chance = 76,
    },
    {
        id = 2560,
        chance = 72,
    },
    {
        id = 2114,
        chance = 72,
    },
}

mtype:register(monster)
