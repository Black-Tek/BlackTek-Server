local mtype = Game.createMonsterType("Zevelon Duskbringer")
local monster = {}

monster.name = "Zevelon Duskbringer"
monster.description = "Zevelon Duskbringer"
monster.experience = 1800
monster.race = "undead"
monster.maxHealth = 1400
monster.health = 1400
monster.speed = 310
monster.manaCost = 0
monster.corpse = 8937
monster.outfit = { lookType = 287 }
monster.changeTarget = {
    interval = 5000,
    chance = 10,
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
        minDamage = 0,
        maxDamage = -282,
    },
    {
        name = "death",
        interval = 1000,
        chance = 12,
        minDamage = 0,
        maxDamage = -200,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        target = true,
        speed = -700,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 1000,
        chance = 12,
        minDamage = 100,
        maxDamage = 235,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 3000,
        chance = 25,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 4500,
        chance = 30,
        duration = 4000,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -15},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Vampire", interval = 3000, chance = 40, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Human blood is not suitable for drinking", yell = false},
    {text = "Your short live is coming to an end", yell = false},
    {text = "Ashari Mortals. Come and stay forever!", yell = false},
}
monster.loot = {
    {
        id = "black pearl",
        chance = 8000,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 75,
    },
    {
        id = "platinum coin",
        chance = 50000,
        maxCount = 5,
    },
    {
        id = "ring of healing",
        chance = 11111,
    },
    {
        id = "vampire shield",
        chance = 4500,
    },
    {
        id = "strong health potion",
        chance = 4000,
    },
    {
        id = "vampire lord token",
        chance = 100000,
    },
}

mtype:register(monster)
