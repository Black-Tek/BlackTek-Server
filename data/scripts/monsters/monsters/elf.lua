local mtype = Game.createMonsterType("Elf")
local monster = {}

monster.name = "Elf"
monster.description = "an elf"
monster.experience = 42
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 190
monster.manaCost = 320
monster.corpse = 6003
monster.outfit = { lookType = 62 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
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
        maxDamage = -15,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -25,
        shootEffect = CONST_ANI_ARROW,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ulathil beia Thratha!", yell = false},
    {text = "Bahaha aka!", yell = false},
    {text = "You are not welcome here.", yell = false},
    {text = "Flee as long as you can.", yell = false},
    {text = "Death to the defilers!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 44661,
        maxCount = 30,
    },
    {
        id = 8839,
        chance = 20013,
        maxCount = 2,
    },
    {
        id = 2482,
        chance = 15055,
    },
    {
        id = 2643,
        chance = 11113,
    },
    {
        id = 2397,
        chance = 10918,
    },
    {
        id = 2484,
        chance = 9010,
    },
    {
        id = 2510,
        chance = 8957,
    },
    {
        id = 2544,
        chance = 7026,
        maxCount = 3,
    },
    {
        id = 10552,
        chance = 1984,
    },
    {
        id = 5921,
        chance = 1011,
    },
}

mtype:register(monster)
