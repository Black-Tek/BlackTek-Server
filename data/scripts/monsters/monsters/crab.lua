local mtype = Game.createMonsterType("Crab")
local monster = {}

monster.name = "Crab"
monster.experience = 30
monster.race = "blood"
monster.maxHealth = 55
monster.health = 55
monster.speed = 144
monster.manaCost = 305
monster.corpse = 6039
monster.outfit = { lookType = 112 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -20,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 80000,
        maxCount = 10,
    },
    {
        id = "crab pincers",
        chance = 20000,
    },
    {
        id = 2667,
        chance = 20000,
    },
}

mtype:register(monster)
