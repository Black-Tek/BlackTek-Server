local mtype = Game.createMonsterType("Cave Rat")
local monster = {}

monster.name = "Cave Rat"
monster.description = "a cave rat"
monster.experience = 10
monster.race = "blood"
monster.maxHealth = 30
monster.health = 30
monster.speed = 150
monster.manaCost = 250
monster.corpse = 5964
monster.outfit = { lookType = 56 }
monster.runHealth = 3
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
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
        maxDamage = -10,
    },
}
monster.defenses = {
    defense = 1,
    armor = 1,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Meeeeep!", yell = false},
    {text = "Meep!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 85000,
        maxCount = 2,
    },
    {
        id = 2696,
        chance = 30000,
    },
    {
        id = "worm",
        chance = 10000,
        maxCount = 2,
    },
    {
        id = "cookie",
        chance = 940,
    },
}

mtype:register(monster)
