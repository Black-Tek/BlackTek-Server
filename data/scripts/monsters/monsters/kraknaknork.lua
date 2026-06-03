local mtype = Game.createMonsterType("Kraknaknork")
local monster = {}

monster.name = "Kraknaknork"
monster.description = "Kraknaknork"
monster.experience = 300
monster.race = "blood"
monster.maxHealth = 80
monster.health = 80
monster.speed = 380
monster.manaCost = 220
monster.corpse = 6017
monster.outfit = { lookType = 74 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 5000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = false,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = 0,
    },
}
monster.defenses = {
    defense = 2,
    armor = 1,
}
monster.loot = {
    {
        id = "meat",
        chance = 20000,
        maxCount = 2,
    },
    {
        id = 2684,
        chance = 9000,
        maxCount = 2,
    },
}

mtype:register(monster)
