local mtype = Game.createMonsterType("Penguin")
local monster = {}

monster.name = "Penguin"
monster.description = "a penguin"
monster.experience = 1
monster.race = "blood"
monster.maxHealth = 33
monster.health = 33
monster.speed = 180
monster.manaCost = 300
monster.corpse = 7334
monster.outfit = { lookType = 250 }
monster.runHealth = 32
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
        maxDamage = -3,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
}
monster.loot = {
    {
        id = 2667,
        chance = 7500,
        maxCount = 2,
    },
    {
        id = "rainbow trout",
        chance = 60,
    },
    {
        id = "green perch",
        chance = 140,
    },
}

mtype:register(monster)
