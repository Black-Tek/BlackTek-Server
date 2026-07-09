local mtype = Game.createMonsterType("Zomba")
local monster = {}

monster.name = "Zomba"
monster.description = "Zomba"
monster.experience = 300
monster.race = "blood"
monster.maxHealth = 300
monster.health = 300
monster.speed = 180
monster.manaCost = 0
monster.corpse = 5986
monster.outfit = { lookType = 41 }
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
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -40,
    },
}
monster.defenses = {
    defense = 13,
    armor = 6,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = -8},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Groarrr!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 25,
    },
    {
        id = "platinum coin",
        chance = 25000,
    },
    {
        id = "life ring",
        chance = 12500,
    },
    {
        id = "lion's mane",
        chance = 100000,
        maxCount = 2,
    },
}

mtype:register(monster)
