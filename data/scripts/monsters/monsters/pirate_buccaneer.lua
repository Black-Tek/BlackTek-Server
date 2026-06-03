local mtype = Game.createMonsterType("Pirate Buccaneer")
local monster = {}

monster.name = "Pirate Buccaneer"
monster.description = "a pirate buccaneer"
monster.experience = 250
monster.race = "blood"
monster.maxHealth = 425
monster.health = 425
monster.speed = 220
monster.manaCost = 595
monster.corpse = 20471
monster.outfit = { lookType = 97 }
monster.runHealth = 50
monster.changeTarget = {
    interval = 4000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
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
        maxDamage = -160,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 4,
        minDamage = 0,
        maxDamage = -100,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Give up!", yell = false},
    {text = "Hiyaa", yell = false},
    {text = "Plundeeeeer!", yell = false},
}
monster.loot = {
    {
        id = 2050,
        chance = 10190,
    },
    {
        id = "gold coin",
        chance = 67740,
        maxCount = 59,
    },
    {
        id = 2238,
        chance = 9900,
    },
    {
        id = "sabre",
        chance = 10100,
    },
    {
        id = "throwing knife",
        chance = 9000,
        maxCount = 5,
    },
    {
        id = "plate armor",
        chance = 1130,
    },
    {
        id = "battle shield",
        chance = 3850,
    },
    {
        id = 5091,
        chance = 1000,
    },
    {
        id = "rum flask",
        chance = 120,
    },
    {
        id = 5792,
        chance = 40,
    },
    {
        id = "pirate backpack",
        chance = 430,
    },
    {
        id = "pirate shirt",
        chance = 1200,
    },
    {
        id = "hook",
        chance = 450,
    },
    {
        id = "eye patch",
        chance = 420,
    },
    {
        id = "peg leg",
        chance = 510,
    },
    {
        id = "strong health potion",
        chance = 670,
    },
    {
        id = "compass",
        chance = 9780,
    },
}

mtype:register(monster)
