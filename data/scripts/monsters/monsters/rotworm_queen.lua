local mtype = Game.createMonsterType("Rotworm Queen")
local monster = {}

monster.name = "Rotworm Queen"
monster.description = "a rotworm queen"
monster.experience = 75
monster.race = "blood"
monster.maxHealth = 105
monster.health = 105
monster.speed = 126
monster.manaCost = 0
monster.corpse = 8947
monster.outfit = { lookType = 295 }
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
}
monster.defenses = {
    defense = 15,
    armor = 10,
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 47,
    },
    {
        id = "worm",
        chance = 20000,
        maxCount = 45,
    },
    {
        id = "gland",
        chance = 3333,
    },
}

mtype:register(monster)
