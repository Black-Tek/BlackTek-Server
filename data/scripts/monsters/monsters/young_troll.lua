local mtype = Game.createMonsterType("Young Troll")
local monster = {}

monster.name = "Young Troll"
monster.description = "a young troll"
monster.experience = 12
monster.race = "blood"
monster.maxHealth = 30
monster.health = 30
monster.speed = 380
monster.manaCost = 0
monster.corpse = 5960
monster.outfit = { lookType = 15 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 5000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
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
    defense = 2,
    armor = 1,
}
monster.loot = {
    {
        id = 2120,
        chance = 10000,
    },
    {
        id = "gold coin",
        chance = 58000,
        maxCount = 12,
    },
    {
        id = "studded club",
        chance = 3000,
    },
    {
        id = "meat",
        chance = 14000,
    },
}

mtype:register(monster)
