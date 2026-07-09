local mtype = Game.createMonsterType("Arachir The Ancient One")
local monster = {}

monster.name = "Arachir The Ancient One"
monster.description = "Arachir The Ancient One"
monster.experience = 1800
monster.race = "undead"
monster.maxHealth = 1600
monster.health = 1600
monster.speed = 300
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
        interval = 9000,
        chance = 100,
        minDamage = -120,
        maxDamage = -300,
        radius = 3,
        target = false,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "death",
        interval = 1000,
        chance = 12,
        minDamage = 0,
        maxDamage = -120,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
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
monster.maxSummons = 2
monster.summons = {
    {name = "Lich", interval = 4500, chance = 100, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I was the shadow that haunted the cradle of humanity!", yell = false},
    {text = "I exist since eons and you want to defy me?", yell = false},
    {text = "Can you feel the passage of time, mortal?", yell = false},
}
monster.loot = {
    {
        id = "black pearl",
        chance = 8980,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 98,
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
        id = 2229,
        chance = 10000,
    },
    {
        id = "vampire shield",
        chance = 6300,
    },
    {
        id = "bloody edge",
        chance = 1200,
    },
    {
        id = "strong health potion",
        chance = 10000,
    },
    {
        id = "vampire lord token",
        chance = 100000,
    },
}

mtype:register(monster)
