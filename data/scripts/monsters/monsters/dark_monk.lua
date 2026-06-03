local mtype = Game.createMonsterType("Dark Monk")
local monster = {}

monster.name = "Dark Monk"
monster.description = "a dark monk"
monster.experience = 145
monster.race = "blood"
monster.maxHealth = 190
monster.health = 190
monster.speed = 230
monster.manaCost = 480
monster.corpse = 20562
monster.outfit = { lookType = 225 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
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
        maxDamage = -100,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -20,
        maxDamage = -50,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 22,
    armor = 22,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 25,
        maxDamage = 49,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 40},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You are no match to us!", yell = false},
    {text = "This is where your path will end!", yell = false},
    {text = "Your end has come!", yell = false},
}
monster.loot = {
    {
        id = "bread",
        chance = 20550,
    },
    {
        id = "gold coin",
        chance = 14600,
        maxCount = 18,
    },
    {
        id = "dark rosary",
        chance = 10500,
    },
    {
        id = "rope belt",
        chance = 6666,
    },
    {
        id = "book of prayers",
        chance = 1900,
    },
    {
        id = 1949,
        chance = 1790,
    },
    {
        id = "life crystal",
        chance = 990,
    },
    {
        id = "safety pin",
        chance = 990,
    },
    {
        id = "ankh",
        chance = 900,
    },
    {
        id = "mana potion",
        chance = 850,
    },
    {
        id = "sandals",
        chance = 850,
    },
    {
        id = 2044,
        chance = 550,
    },
    {
        id = "brown flask",
        chance = 390,
    },
    {
        id = "power ring",
        chance = 120,
    },
}

mtype:register(monster)
