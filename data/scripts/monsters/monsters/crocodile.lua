local mtype = Game.createMonsterType("Crocodile")
local monster = {}

monster.name = "Crocodile"
monster.description = "a crocodile"
monster.experience = 40
monster.race = "blood"
monster.maxHealth = 105
monster.health = 105
monster.speed = 156
monster.manaCost = 350
monster.corpse = 6046
monster.outfit = { lookType = 119 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 5000,
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -40,
    },
}
monster.defenses = {
    defense = 13,
    armor = 13,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
}
monster.loot = {
    {
        id = 2148,
        chance = 49805,
        maxCount = 10,
    },
    {
        id = 2671,
        chance = 39795,
    },
    {
        id = 11196,
        chance = 20032,
    },
    {
        id = 3982,
        chance = 86,
    },
}

mtype:register(monster)
