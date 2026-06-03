local mtype = Game.createMonsterType("Pig")
local monster = {}

monster.name = "Pig"
monster.description = "a pig"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 25
monster.health = 25
monster.speed = 114
monster.manaCost = 255
monster.corpse = 6000
monster.outfit = { lookType = 60 }
monster.runHealth = 25
monster.changeTarget = {
    interval = 4000,
    chance = 0,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Oink oink", yell = false},
    {text = "Oink", yell = false},
}
monster.loot = {
    {
        id = "meat",
        chance = 65000,
        maxCount = 4,
    },
    {
        id = "pig foot",
        chance = 1200,
    },
}

mtype:register(monster)
