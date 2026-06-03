local mtype = Game.createMonsterType("Donkey")
local monster = {}

monster.name = "Donkey"
monster.description = "a donkey"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 45
monster.health = 45
monster.speed = 150
monster.manaCost = 0
monster.corpse = 13509
monster.outfit = { lookType = 399 }
monster.runHealth = 25
monster.changeTarget = {
    interval = 2000,
    chance = 20,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
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
        maxDamage = -5,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grunt!", yell = false},
}
monster.loot = {
    {
        id = "meat",
        chance = 20000,
        maxCount = 2,
    },
}

mtype:register(monster)
