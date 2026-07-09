local mtype = Game.createMonsterType("Squirrel")
local monster = {}

monster.name = "Squirrel"
monster.description = "a squirrel"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 20
monster.health = 20
monster.speed = 480
monster.manaCost = 220
monster.corpse = 7628
monster.outfit = { lookType = 274 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
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
    defense = 1,
    armor = 1,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Chchch", yell = false},
}
monster.loot = {
    {
        id = 11213,
        chance = 36613,
    },
    {
        id = 7910,
        chance = 652,
    },
    {
        id = 7909,
        chance = 516,
    },
}

mtype:register(monster)
