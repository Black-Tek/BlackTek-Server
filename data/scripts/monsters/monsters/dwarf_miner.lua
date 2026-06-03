local mtype = Game.createMonsterType("Dwarf Miner")
local monster = {}

monster.name = "Dwarf Miner"
monster.description = "a dwarf miner"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 120
monster.health = 120
monster.speed = 170
monster.manaCost = 420
monster.corpse = 6007
monster.outfit = {
    lookType = 160,
    lookHead = 77,
    lookBody = 100,
    lookLegs = 97,
    lookFeet = 116,
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
        maxDamage = -35,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Work, work!", yell = false},
    {text = "Intruders in the mines!", yell = false},
    {text = "Mine, all mine!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 41358,
        maxCount = 10,
    },
    {
        id = 2386,
        chance = 15123,
    },
    {
        id = 3456,
        chance = 12037,
    },
    {
        id = 2649,
        chance = 8951,
    },
    {
        id = 2484,
        chance = 7099,
    },
    {
        id = 2666,
        chance = 3704,
    },
    {
        id = 2213,
        chance = 617,
    },
    {
        id = 5880,
        chance = 617,
    },
}

mtype:register(monster)
