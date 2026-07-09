local mtype = Game.createMonsterType("Big Boss Trolliver")
local monster = {}

monster.name = "Big Boss Trolliver"
monster.description = "Big Boss Trolliver"
monster.experience = 105
monster.race = "blood"
monster.maxHealth = 150
monster.health = 150
monster.speed = 140
monster.manaCost = 0
monster.corpse = 7926
monster.outfit = { lookType = 281 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 2000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
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
        maxDamage = -90,
    },
}
monster.defenses = {
    defense = 12,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 22},
    {type = COMBAT_HOLYDAMAGE, percent = 11},
    {type = COMBAT_EARTHDAMAGE, percent = -3},
    {type = COMBAT_DEATHDAMAGE, percent = -4},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 6000,
        maxCount = 57,
    },
    {
        id = "spear",
        chance = 2000,
        maxCount = 2,
    },
    {
        id = "studded club",
        chance = 5000,
    },
    {
        id = "meat",
        chance = 1500,
        maxCount = 3,
    },
}

mtype:register(monster)
