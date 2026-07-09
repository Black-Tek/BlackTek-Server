local mtype = Game.createMonsterType("Xenia")
local monster = {}

monster.name = "Xenia"
monster.description = "Xenia"
monster.experience = 255
monster.race = "blood"
monster.maxHealth = 200
monster.health = 200
monster.speed = 176
monster.manaCost = 0
monster.corpse = 20542
monster.outfit = {
    lookType = 137,
    lookHead = 95,
    lookBody = 115,
    lookLegs = 115,
    lookFeet = 95,
}
monster.runHealth = 10
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -50,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = -28,
        maxDamage = -42,
        shootEffect = CONST_ANI_SPEAR,
    },
}
monster.defenses = {
    defense = 14,
    armor = 12,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 7},
    {type = COMBAT_DEATHDAMAGE, percent = -7},
    {type = COMBAT_PHYSICALDAMAGE, percent = -6},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Stand still!", yell = false},
    {text = "One more head for me!", yell = false},
    {text = "Head off!", yell = false},
}
monster.loot = {
    {
        id = "crystal necklace",
        chance = 2500,
    },
    {
        id = "small diamond",
        chance = 2500,
        maxCount = 2,
    },
    {
        id = "small ruby",
        chance = 2500,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 45,
    },
    {
        id = "protection amulet",
        chance = 1000,
    },
    {
        id = "double axe",
        chance = 3333,
    },
    {
        id = "machete",
        chance = 10000,
    },
    {
        id = "chain helmet",
        chance = 5000,
    },
    {
        id = "plate armor",
        chance = 1333,
    },
    {
        id = "chain armor",
        chance = 6666,
    },
    {
        id = "meat",
        chance = 20000,
        maxCount = 3,
    },
    {
        id = "red apple",
        chance = 10000,
        maxCount = 4,
    },
    {
        id = "worm",
        chance = 50000,
        maxCount = 10,
    },
}

mtype:register(monster)
