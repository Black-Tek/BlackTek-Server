local mtype = Game.createMonsterType("Quara Mantassin Scout")
local monster = {}

monster.name = "Quara Mantassin Scout"
monster.description = "a quara mantassin scout"
monster.experience = 100
monster.race = "blood"
monster.maxHealth = 220
monster.health = 220
monster.speed = 140
monster.manaCost = 480
monster.corpse = 6064
monster.outfit = { lookType = 72 }
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
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
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
    defense = 7,
    armor = 7,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "lifedrain", combat = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zuerk Pachak!", yell = false},
    {text = "Shrrrr", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 93987,
        maxCount = 30,
    },
    {
        id = 12445,
        chance = 7916,
    },
    {
        id = 2464,
        chance = 4990,
    },
    {
        id = 2146,
        chance = 996,
    },
    {
        id = 2229,
        chance = 979,
    },
    {
        id = 2377,
        chance = 802,
    },
    {
        id = 2165,
        chance = 711,
    },
    {
        id = 5895,
        chance = 365,
    },
}

mtype:register(monster)
