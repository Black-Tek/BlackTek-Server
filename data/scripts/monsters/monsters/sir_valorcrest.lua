local mtype = Game.createMonsterType("Sir Valorcrest")
local monster = {}

monster.name = "Sir Valorcrest"
monster.description = "Sir Valorcrest"
monster.experience = 1800
monster.race = "undead"
monster.maxHealth = 1600
monster.health = 1600
monster.speed = 270
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
        maxDamage = -380,
    },
    {
        name = "death",
        interval = 1000,
        chance = 12,
        minDamage = 0,
        maxDamage = -190,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 35,
    armor = 38,
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
monster.maxSummons = 4
monster.summons = {
    {name = "Vampire", interval = 2000, chance = 30, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I challenge you!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 93,
    },
    {
        id = "platinum coin",
        chance = 50000,
        maxCount = 5,
    },
    {
        id = "sword ring",
        chance = 1400,
    },
    {
        id = "ring of healing",
        chance = 17111,
    },
    {
        id = 2229,
        chance = 15000,
    },
    {
        id = "vampire shield",
        chance = 6300,
    },
    {
        id = "chaos mace",
        chance = 250,
    },
    {
        id = "strong health potion",
        chance = 1500,
    },
    {
        id = "vampire lord token",
        chance = 100000,
    },
    {
        id = "vampire lord token",
        chance = 100000,
    },
}

mtype:register(monster)
