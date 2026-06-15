local mtype = Game.createMonsterType("Munster")
local monster = {}

monster.name = "Munster"
monster.description = "Munster"
monster.experience = 35
monster.race = "blood"
monster.maxHealth = 58
monster.health = 58
monster.speed = 200
monster.manaCost = 250
monster.corpse = 2813
monster.outfit = { lookType = 56 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    boss = true,
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
    defense = 4,
    armor = 2,
}
monster.maxSummons = 2
monster.summons = {
    {name = "Rat", interval = 2000, chance = 20, max = 2},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Meeeeep!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 16,
    },
    {
        id = "bone club",
        chance = 5000,
    },
    {
        id = "jacket",
        chance = 50000,
    },
    {
        id = "cookie",
        chance = 5000,
    },
    {
        id = 2696,
        chance = 50000,
    },
}

mtype:register(monster)
