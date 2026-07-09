local mtype = Game.createMonsterType("Blood Crab")
local monster = {}

monster.name = "Blood Crab"
monster.description = "a blood crab"
monster.experience = 160
monster.race = "blood"
monster.maxHealth = 290
monster.health = 290
monster.speed = 160
monster.manaCost = 505
monster.corpse = 6075
monster.outfit = { lookType = 200 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
    },
}
monster.defenses = {
    defense = 28,
    armor = 28,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 85818,
        maxCount = 20,
    },
    {
        id = 2667,
        chance = 14803,
    },
    {
        id = 10550,
        chance = 6053,
    },
    {
        id = 2464,
        chance = 5122,
    },
    {
        id = 2478,
        chance = 2143,
    },
    {
        id = 2143,
        chance = 517,
    },
}

mtype:register(monster)
