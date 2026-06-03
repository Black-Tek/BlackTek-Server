local mtype = Game.createMonsterType("Corym Vanguard")
local monster = {}

monster.name = "Corym Vanguard"
monster.description = "a corym vanguard"
monster.experience = 490
monster.race = "blood"
monster.maxHealth = 700
monster.health = 700
monster.speed = 210
monster.manaCost = 0
monster.corpse = 19734
monster.outfit = {
    lookType = 534,
    lookHead = 0,
    lookBody = 19,
    lookLegs = 121,
    lookFeet = 0,
}
monster.runHealth = 50
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -140,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -50,
        maxDamage = -100,
        length = 5,
        spread = 3,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -40,
        maxDamage = -70,
        radius = 4,
        target = true,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 29,
    armor = 29,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 30,
        maxDamage = 60,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gimme! Gimme!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 20,
    },
    {
        id = 2696,
        chance = 20000,
    },
    {
        id = "bola",
        chance = 10000,
    },
    {
        id = "spike shield",
        chance = 4761,
    },
    {
        id = "ratana",
        chance = 5000,
    },
    {
        id = "life preserver",
        chance = 5000,
    },
    {
        id = "cheese cutter",
        chance = 16666,
    },
    {
        id = "cheesy figurine",
        chance = 3846,
    },
    {
        id = "earflap",
        chance = 11111,
    },
    {
        id = "soft cheese",
        chance = 14285,
    },
    {
        id = "rat cheese",
        chance = 14285,
    },
    {
        id = "rat god doll",
        chance = 14,
    },
    {
        id = "leather harness",
        chance = 1612,
    },
    {
        id = "spiky club",
        chance = 3225,
    },
}

mtype:register(monster)
