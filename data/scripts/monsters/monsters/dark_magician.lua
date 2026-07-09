local mtype = Game.createMonsterType("Dark Magician")
local monster = {}

monster.name = "Dark Magician"
monster.description = "a dark magician"
monster.experience = 185
monster.race = "blood"
monster.maxHealth = 325
monster.health = 325
monster.speed = 210
monster.manaCost = 0
monster.corpse = 20367
monster.outfit = {
    lookType = 133,
    lookHead = 58,
    lookBody = 95,
    lookLegs = 51,
    lookFeet = 131,
    lookAddons = 2,
}
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -40,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -5,
        maxDamage = -40,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -20,
        maxDamage = -30,
        shootEffect = CONST_ANI_ENERGY,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 60,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Feel the power of my runes!", yell = false},
    {text = "Killing you is getting expensive!", yell = false},
    {text = "My secrets are mine alone!", yell = false},
    {text = "Stand Still!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 75100,
        maxCount = 55,
    },
    {
        id = "wand of decay",
        chance = 280,
    },
    {
        id = "blank rune",
        chance = 10000,
    },
    {
        id = "strong health potion",
        chance = 3000,
    },
    {
        id = "strong mana potion",
        chance = 2860,
    },
    {
        id = "health potion",
        chance = 12000,
    },
    {
        id = "mana potion",
        chance = 11900,
    },
    {
        id = "small enchanted emerald",
        chance = 510,
    },
    {
        id = "reins",
        chance = 20,
    },
}

mtype:register(monster)
