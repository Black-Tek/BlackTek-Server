local mtype = Game.createMonsterType("Vampire Viscount")
local monster = {}

monster.name = "Vampire Viscount"
monster.description = "a vampire viscount"
monster.experience = 800
monster.race = "blood"
monster.maxHealth = 1200
monster.health = 1200
monster.speed = 220
monster.manaCost = 0
monster.corpse = 21275
monster.outfit = { lookType = 555 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        range = 6,
        minDamage = -50,
        maxDamage = -100,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 10,
        minDamage = -320,
        maxDamage = -560,
        radius = 6,
        effect = CONST_ME_BATS,
    },
}
monster.defenses = {
    defense = 42,
    armor = 42,
    {
        name = "outfit",
        interval = 2000,
        chance = 10,
        duration = 4000,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Prepare to BLEED!", yell = false},
    {text = "Don't struggle. We don't want to waste a drop of blood now, do we?", yell = false},
    {text = "Ah, refreshments have arrived!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 84797,
        maxCount = 50,
    },
    {
        id = 10602,
        chance = 7712,
    },
    {
        id = 7589,
        chance = 7447,
    },
    {
        id = 7588,
        chance = 6103,
    },
    {
        id = 21241,
        chance = 5321,
    },
    {
        id = 21244,
        chance = 3922,
    },
    {
        id = 2147,
        chance = 3338,
        maxCount = 2,
    },
    {
        id = 12405,
        chance = 2930,
    },
    {
        id = 2144,
        chance = 2666,
    },
    {
        id = 2396,
        chance = 815,
    },
    {
        id = 2156,
        chance = 364,
    },
    {
        id = 5911,
        chance = 275,
    },
    {
        id = 2534,
        chance = 209,
    },
}

mtype:register(monster)
