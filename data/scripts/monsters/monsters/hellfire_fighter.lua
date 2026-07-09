local mtype = Game.createMonsterType("Hellfire Fighter")
local monster = {}

monster.name = "Hellfire Fighter"
monster.description = "a hellfire fighter"
monster.experience = 3900
monster.race = "fire"
monster.maxHealth = 3800
monster.health = 3800
monster.speed = 220
monster.manaCost = 0
monster.corpse = 6324
monster.outfit = { lookType = 243 }
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -520,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 3,
        target = false,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -392,
        maxDamage = -1500,
        length = 8,
        target = false,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -330,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "hellfire fighter soulfire",
        interval = 2000,
        chance = 15,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "emerald bangle",
        chance = 2200,
    },
    {
        id = 2136,
        chance = 190,
    },
    {
        id = "small diamond",
        chance = 1400,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 10000,
        maxCount = 46,
    },
    {
        id = "bronze amulet",
        chance = 1000,
    },
    {
        id = "wand of inferno",
        chance = 9450,
    },
    {
        id = "burnt scroll",
        chance = 50000,
    },
    {
        id = "blank rune",
        chance = 30000,
        maxCount = 2,
    },
    {
        id = "fire sword",
        chance = 4140,
    },
    {
        id = "fire axe",
        chance = 440,
    },
    {
        id = "soul orb",
        chance = 12150,
    },
    {
        id = "demonic essence",
        chance = 14500,
    },
    {
        id = "magma legs",
        chance = 730,
    },
    {
        id = "magma coat",
        chance = 470,
    },
    {
        id = "fiery heart",
        chance = 9570,
    },
    {
        id = "piece of hellfire armor",
        chance = 5060,
    },
    {
        id = 13757,
        chance = 670,
    },
}

mtype:register(monster)
