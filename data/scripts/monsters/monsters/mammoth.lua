local mtype = Game.createMonsterType("Mammoth")
local monster = {}

monster.name = "Mammoth"
monster.description = "a mammoth"
monster.experience = 160
monster.race = "blood"
monster.maxHealth = 320
monster.health = 320
monster.speed = 190
monster.manaCost = 500
monster.corpse = 6074
monster.outfit = { lookType = 199 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
        maxDamage = -110,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Troooooot!", yell = false},
    {text = "Hooooot-Toooooot!", yell = false},
    {text = "Tooooot.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 89882,
        maxCount = 40,
    },
    {
        id = 2666,
        chance = 40278,
    },
    {
        id = 2671,
        chance = 29949,
    },
    {
        id = 11224,
        chance = 7669,
    },
    {
        id = 11238,
        chance = 7462,
        maxCount = 2,
    },
    {
        id = 7381,
        chance = 2507,
    },
    {
        id = 7432,
        chance = 526,
    },
    {
        id = 3973,
        chance = 494,
    },
}

mtype:register(monster)
