local mtype = Game.createMonsterType("Deepsea Blood Crab")
local monster = {}

monster.name = "Deepsea Blood Crab"
monster.description = "a deepsea blood crab"
monster.experience = 180
monster.race = "blood"
monster.maxHealth = 320
monster.health = 320
monster.speed = 380
monster.manaCost = 0
monster.corpse = 6075
monster.outfit = { lookType = 200 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
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
        maxDamage = -115,
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
        chance = 86909,
        maxCount = 20,
    },
    {
        id = 2667,
        chance = 10851,
    },
    {
        id = 10550,
        chance = 6816,
    },
    {
        id = 2464,
        chance = 5241,
    },
    {
        id = 2478,
        chance = 2707,
    },
    {
        id = 2143,
        chance = 541,
    },
}

mtype:register(monster)
