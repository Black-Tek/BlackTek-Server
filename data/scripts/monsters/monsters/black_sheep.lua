local mtype = Game.createMonsterType("Black Sheep")
local monster = {}

monster.name = "Black Sheep"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 20
monster.health = 20
monster.speed = 116
monster.manaCost = 250
monster.corpse = 5994
monster.outfit = { lookType = 13 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 4000,
    chance = 20,
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
    defense = 1,
    armor = 1,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Maeh", yell = false},
}
monster.loot = {
    {
        id = "meat",
        chance = 55380,
        maxCount = 5,
    },
    {
        id = "black wool",
        chance = 620,
    },
}

mtype:register(monster)
