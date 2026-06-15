local mtype = Game.createMonsterType("Dwarf")
local monster = {}

monster.name = "Dwarf"
monster.description = "a dwarf"
monster.experience = 45
monster.race = "blood"
monster.maxHealth = 90
monster.health = 90
monster.speed = 170
monster.manaCost = 320
monster.corpse = 6007
monster.outfit = { lookType = 69 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
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
        maxDamage = -30,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hail Durin!", yell = false},
}
monster.loot = {
    {
        id = 2787,
        chance = 50140,
    },
    {
        id = 2148,
        chance = 35361,
        maxCount = 8,
    },
    {
        id = 2388,
        chance = 24739,
    },
    {
        id = 2386,
        chance = 14596,
    },
    {
        id = 3456,
        chance = 10143,
    },
    {
        id = 2530,
        chance = 10098,
    },
    {
        id = 2649,
        chance = 9682,
    },
    {
        id = 2484,
        chance = 8017,
    },
    {
        id = 2597,
        chance = 7929,
    },
    {
        id = 2213,
        chance = 95,
    },
    {
        id = 5880,
        chance = 83,
    },
}

mtype:register(monster)
