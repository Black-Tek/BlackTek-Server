local mtype = Game.createMonsterType("Killer Caiman")
local monster = {}

monster.name = "Killer Caiman"
monster.description = "a killer caiman"
monster.experience = 900
monster.race = "blood"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 186
monster.manaCost = 0
monster.corpse = 11430
monster.outfit = { lookType = 358 }
monster.runHealth = 10
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
    illusionable = false,
    convinceable = false,
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
        maxDamage = -180,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 700,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 99997,
        maxCount = 180,
    },
    {
        id = 2671,
        chance = 39807,
    },
    {
        id = 11196,
        chance = 24966,
        maxCount = 2,
    },
    {
        id = 2149,
        chance = 9963,
        maxCount = 5,
    },
    {
        id = 2425,
        chance = 4982,
    },
    {
        id = 11245,
        chance = 4828,
        maxCount = 2,
    },
    {
        id = 7632,
        chance = 974,
    },
    {
        id = 3982,
        chance = 486,
    },
}

mtype:register(monster)
