local mtype = Game.createMonsterType("Quara Pincher Scout")
local monster = {}

monster.name = "Quara Pincher Scout"
monster.description = "a quara pincher scout"
monster.experience = 600
monster.race = "blood"
monster.maxHealth = 775
monster.health = 775
monster.speed = 156
monster.manaCost = 0
monster.corpse = 6063
monster.outfit = { lookType = 77 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -240,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 1,
        speed = -600,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Clank clank!", yell = false},
    {text = "Clap!", yell = false},
    {text = "Crrrk! Crrrk!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 89910,
        maxCount = 129,
    },
    {
        id = 12446,
        chance = 9912,
    },
    {
        id = 2463,
        chance = 4965,
    },
    {
        id = 2147,
        chance = 3836,
        maxCount = 2,
    },
    {
        id = 2381,
        chance = 2056,
    },
    {
        id = 5895,
        chance = 808,
    },
    {
        id = 2177,
        chance = 781,
    },
}

mtype:register(monster)
