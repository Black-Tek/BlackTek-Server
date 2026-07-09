local mtype = Game.createMonsterType("The Imperor")
local monster = {}

monster.name = "The Imperor"
monster.description = "The Imperor"
monster.experience = 8000
monster.race = "fire"
monster.maxHealth = 15000
monster.health = 15000
monster.speed = 330
monster.manaCost = 0
monster.corpse = 6364
monster.outfit = { lookType = 237 }
monster.changeTarget = {
    interval = 5000,
    chance = 5,
}
monster.targetDistance = 4
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -660,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 13,
        range = 1,
        minDamage = -50,
        maxDamage = -800,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -50,
        maxDamage = -800,
        radius = 6,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 30,
        minDamage = -90,
        maxDamage = -350,
        length = 4,
        spread = 2,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 50,
        minDamage = 175,
        maxDamage = 505,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 12,
        speed = 380,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 3000,
        chance = 20,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -20},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "diabolic imp", interval = 5500, chance = 30, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Muahaha!", yell = false},
    {text = "He he he!", yell = false},
}
monster.loot = {
    {
        id = 2050,
        chance = 15000,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 79,
    },
    {
        id = "small amethyst",
        chance = 8888,
    },
    {
        id = "stealth ring",
        chance = 4555,
    },
    {
        id = "necrotic rod",
        chance = 833,
    },
    {
        id = "blank rune",
        chance = 18000,
    },
    {
        id = "chain armor",
        chance = 7887,
    },
    {
        id = "brass armor",
        chance = 6577,
    },
    {
        id = "brass armor",
        chance = 6577,
    },
    {
        id = "pitchfork",
        chance = 20000,
    },
    {
        id = "soul orb",
        chance = 5666,
    },
    {
        id = 6300,
        chance = 999,
    },
    {
        id = "demonic essence",
        chance = 7777,
    },
    {
        id = "infernal bolt",
        chance = 5000,
        maxCount = 5,
    },
    {
        id = "the Imperor's trident",
        chance = 100000,
    },
    {
        id = "magma boots",
        chance = 2000,
    },
    {
        id = "magma coat",
        chance = 2000,
    },
    {
        id = "magma monocle",
        chance = 2000,
    },
}

mtype:register(monster)
