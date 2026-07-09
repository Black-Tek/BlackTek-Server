local mtype = Game.createMonsterType("Troll Guard")
local monster = {}

monster.name = "Troll Guard"
monster.description = "a troll guard"
monster.experience = 25
monster.race = "blood"
monster.maxHealth = 60
monster.health = 60
monster.speed = 180
monster.manaCost = 0
monster.corpse = 7926
monster.outfit = { lookType = 281 }
monster.runHealth = 17
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
    illusionable = true,
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
        maxDamage = -15,
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
        maxCount = 2,
    },
}

mtype:register(monster)
