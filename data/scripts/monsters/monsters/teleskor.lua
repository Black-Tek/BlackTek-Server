local mtype = Game.createMonsterType("Teleskor")
local monster = {}

monster.name = "Teleskor"
monster.description = "Teleskor"
monster.experience = 70
monster.race = "undead"
monster.maxHealth = 80
monster.health = 80
monster.speed = 150
monster.manaCost = 0
monster.corpse = 5972
monster.outfit = { lookType = 298 }
monster.changeTarget = {
    interval = 4000,
    chance = 5,
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
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -30,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Who disturbs my slumber?", yell = false},
    {text = "Mourn the dead, do not hunt them!", yell = false},
}
monster.loot = {
    {
        id = 12437,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 81000,
        maxCount = 79,
    },
    {
        id = 2398,
        chance = 72000,
    },
    {
        id = 2473,
        chance = 72000,
    },
    {
        id = 2511,
        chance = 45000,
    },
    {
        id = 2050,
        chance = 36000,
    },
    {
        id = 2388,
        chance = 27000,
    },
    {
        id = 2376,
        chance = 27000,
    },
}

mtype:register(monster)
