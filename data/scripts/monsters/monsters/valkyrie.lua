local mtype = Game.createMonsterType("Valkyrie")
local monster = {}

monster.name = "Valkyrie"
monster.description = "a valkyrie"
monster.experience = 85
monster.race = "blood"
monster.maxHealth = 190
monster.health = 190
monster.speed = 176
monster.manaCost = 450
monster.corpse = 20523
monster.outfit = {
    lookType = 139,
    lookHead = 113,
    lookBody = 38,
    lookLegs = 76,
    lookFeet = 96,
}
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -70,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -50,
        shootEffect = CONST_ANI_SPEAR,
    },
}
monster.defenses = {
    defense = 12,
    armor = 12,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Another head for me!", yell = false},
    {text = "Head off!", yell = false},
    {text = "Your head will be mine!", yell = false},
    {text = "Stand still!", yell = false},
    {text = "One more head for me!", yell = false},
}
monster.loot = {
    {
        id = "spear",
        chance = 55000,
        maxCount = 3,
    },
    {
        id = "gold coin",
        chance = 32000,
        maxCount = 12,
    },
    {
        id = "meat",
        chance = 30100,
    },
    {
        id = "chain armor",
        chance = 10000,
    },
    {
        id = "red apple",
        chance = 7500,
        maxCount = 2,
    },
    {
        id = "girlish hair decoration",
        chance = 6000,
    },
    {
        id = "hunting spear",
        chance = 5155,
    },
    {
        id = "protective charm",
        chance = 3200,
    },
    {
        id = "protection amulet",
        chance = 1100,
    },
    {
        id = "plate armor",
        chance = 820,
    },
    {
        id = 2229,
        chance = 740,
    },
    {
        id = "health potion",
        chance = 500,
    },
    {
        id = "double axe",
        chance = 420,
    },
    {
        id = "small diamond",
        chance = 200,
    },
}

mtype:register(monster)
