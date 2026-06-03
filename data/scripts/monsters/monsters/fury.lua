local mtype = Game.createMonsterType("Fury")
local monster = {}

monster.name = "Fury"
monster.description = "a fury"
monster.experience = 4500
monster.race = "blood"
monster.maxHealth = 4100
monster.health = 4100
monster.speed = 250
monster.manaCost = 0
monster.corpse = 20399
monster.outfit = {
    lookType = 149,
    lookHead = 94,
    lookBody = 77,
    lookLegs = 78,
    lookFeet = 79,
    lookAddons = 1,
}
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -510,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -300,
        length = 8,
        spread = 3,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 5,
        minDamage = -120,
        maxDamage = -700,
        length = 8,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -120,
        maxDamage = -300,
        radius = 4,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "fury skill reducer",
        interval = 2000,
        chance = 5,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -120,
        maxDamage = -300,
        radius = 3,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -125,
        maxDamage = -250,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -800,
        duration = 30000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 800,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ahhhhrrrr!", yell = false},
    {text = "Waaaaah!", yell = false},
    {text = "Carnage!", yell = false},
    {text = "Dieee!", yell = false},
}
monster.loot = {
    {
        id = 2120,
        chance = 8000,
        maxCount = 3,
    },
    {
        id = 2124,
        chance = 410,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 38000,
        maxCount = 69,
    },
    {
        id = "platinum coin",
        chance = 2800,
        maxCount = 4,
    },
    {
        id = "terra rod",
        chance = 20000,
    },
    {
        id = "golden legs",
        chance = 130,
    },
    {
        id = "steel boots",
        chance = 790,
    },
    {
        id = "meat",
        chance = 25000,
    },
    {
        id = "orichalcum pearl",
        chance = 1500,
        maxCount = 4,
    },
    {
        id = "red piece of cloth",
        chance = 4000,
    },
    {
        id = "soul orb",
        chance = 21500,
    },
    {
        id = "soul orb",
        chance = 50,
    },
    {
        id = 6301,
        chance = 60,
    },
    {
        id = "demonic essence",
        chance = 22500,
    },
    {
        id = "assassin dagger",
        chance = 660,
    },
    {
        id = "noble axe",
        chance = 2000,
    },
    {
        id = "great health potion",
        chance = 10500,
    },
    {
        id = "jalapeno pepper",
        chance = 29280,
        maxCount = 4,
    },
    {
        id = 9813,
        chance = 1920,
    },
}

mtype:register(monster)
