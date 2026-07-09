local mtype = Game.createMonsterType("Undead Prospector")
local monster = {}

monster.name = "Undead Prospector"
monster.description = "an undead prospector"
monster.experience = 85
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 170
monster.manaCost = 440
monster.corpse = 5976
monster.outfit = { lookType = 18 }
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
    pushable = false,
    canPushItems = true,
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
        maxDamage = -50,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 5,
        maxDamage = 15,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "death", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Our mine... leave us alone.", yell = false},
    {text = "Turn back...", yell = false},
    {text = "These mine is ours... you shall not pass.", yell = false},
}
monster.loot = {
    {
        id = 3976,
        chance = 80636,
        maxCount = 6,
    },
    {
        id = 2148,
        chance = 71225,
        maxCount = 30,
    },
    {
        id = 2050,
        chance = 61070,
    },
    {
        id = 2460,
        chance = 18551,
    },
    {
        id = 2403,
        chance = 14760,
    },
    {
        id = 2473,
        chance = 5349,
    },
    {
        id = 2483,
        chance = 3588,
    },
    {
        id = 2229,
        chance = 2708,
    },
    {
        id = 5913,
        chance = 948,
    },
    {
        id = 2168,
        chance = 135,
    },
}

mtype:register(monster)
