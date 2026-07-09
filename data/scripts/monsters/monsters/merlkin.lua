local mtype = Game.createMonsterType("Merlkin")
local monster = {}

monster.name = "Merlkin"
monster.description = "a merlkin"
monster.experience = 145
monster.race = "blood"
monster.maxHealth = 235
monster.health = 235
monster.speed = 194
monster.manaCost = 0
monster.corpse = 6044
monster.outfit = { lookType = 117 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 70
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -30,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -90,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -15,
        maxDamage = -45,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "poisonfield",
        interval = 2000,
        chance = 15,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 16,
    armor = 16,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 30,
        maxDamage = 40,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -15},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ugh! Ugh! Ugh!", yell = false},
    {text = "Holy banana!", yell = false},
    {text = "Chakka! Chakka!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 82947,
        maxCount = 45,
    },
    {
        id = 2676,
        chance = 30430,
        maxCount = 12,
    },
    {
        id = 2162,
        chance = 3020,
    },
    {
        id = 12467,
        chance = 1918,
    },
    {
        id = 5883,
        chance = 1014,
    },
    {
        id = 2675,
        chance = 1010,
        maxCount = 5,
    },
    {
        id = 2188,
        chance = 980,
    },
    {
        id = 7620,
        chance = 693,
    },
    {
        id = 2150,
        chance = 310,
    },
    {
        id = 3966,
        chance = 102,
    },
}

mtype:register(monster)
