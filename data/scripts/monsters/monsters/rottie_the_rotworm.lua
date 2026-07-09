local mtype = Game.createMonsterType("Rottie The Rotworm")
local monster = {}

monster.name = "Rottie The Rotworm"
monster.description = "Rottie The Rotworm"
monster.experience = 40
monster.race = "blood"
monster.maxHealth = 65
monster.health = 65
monster.speed = 180
monster.manaCost = 0
monster.corpse = 5967
monster.outfit = { lookType = 26 }
monster.changeTarget = {
    interval = 2000,
    chance = 0,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -25,
    },
}
monster.defenses = {
    defense = 11,
    armor = 8,
}
monster.loot = {
    {
        id = 10609,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 84211,
        maxCount = 18,
    },
    {
        id = 2398,
        chance = 63158,
    },
    {
        id = 3976,
        chance = 47368,
        maxCount = 5,
    },
    {
        id = 2376,
        chance = 26316,
    },
    {
        id = 2671,
        chance = 21053,
    },
    {
        id = 2530,
        chance = 15789,
    },
    {
        id = 2666,
        chance = 15789,
    },
    {
        id = 2480,
        chance = 10526,
    },
    {
        id = 2412,
        chance = 5263,
    },
}

mtype:register(monster)
