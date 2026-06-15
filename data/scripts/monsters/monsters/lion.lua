local mtype = Game.createMonsterType("Lion")
local monster = {}

monster.name = "Lion"
monster.description = "a lion"
monster.experience = 30
monster.race = "blood"
monster.maxHealth = 80
monster.health = 80
monster.speed = 190
monster.manaCost = 320
monster.corpse = 5986
monster.outfit = { lookType = 41 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
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
        maxDamage = -40,
    },
}
monster.defenses = {
    defense = 6,
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
        id = 2666,
        chance = 45327,
    },
    {
        id = 2671,
        chance = 19265,
    },
    {
        id = 10608,
        chance = 1148,
    },
}

mtype:register(monster)
