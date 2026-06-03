local mtype = Game.createMonsterType("Sheep")
local monster = {}

monster.name = "Sheep"
monster.description = "a sheep"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 20
monster.health = 20
monster.speed = 116
monster.manaCost = 250
monster.corpse = 5991
monster.outfit = { lookType = 14 }
monster.runHealth = 20
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
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1,
    },
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
        chance = 70000,
        maxCount = 4,
    },
    {
        id = "wool",
        chance = 1000,
    },
}

mtype:register(monster)
