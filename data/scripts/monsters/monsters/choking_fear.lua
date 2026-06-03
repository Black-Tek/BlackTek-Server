local mtype = Game.createMonsterType("Choking Fear")
local monster = {}

monster.name = "Choking Fear"
monster.description = "a choking fear"
monster.experience = 4700
monster.race = "undead"
monster.maxHealth = 5800
monster.health = 5800
monster.speed = 260
monster.manaCost = 0
monster.corpse = 22493
monster.outfit = { lookType = 586 }
monster.changeTarget = {
    interval = 4000,
    chance = 5,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -499,
        -- NOTE: melee condition (poison=600); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 600000,
        },
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -700,
        maxDamage = -900,
        length = 5,
        spread = 3,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = -300,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_SLEEP,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        radius = 1,
        target = true,
        speed = -800,
        duration = 15000,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_SLEEP,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -300,
        radius = 4,
        target = false,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "choking fear drown",
        interval = 2000,
        chance = 20,
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        minDamage = -250,
        maxDamage = -500,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 80,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ah, sweet air... don't you miss it?", yell = false},
    {text = "Murr tat muuza!", yell = false},
    {text = "kchh", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 8,
    },
    {
        id = "energy ring",
        chance = 3000,
    },
    {
        id = "life ring",
        chance = 4200,
    },
    {
        id = "ring of healing",
        chance = 3000,
    },
    {
        id = "guardian shield",
        chance = 1540,
    },
    {
        id = "beastslayer axe",
        chance = 3290,
    },
    {
        id = "brown piece of cloth",
        chance = 6000,
    },
    {
        id = "yellow piece of cloth",
        chance = 3500,
    },
    {
        id = "great mana potion",
        chance = 20000,
        maxCount = 3,
    },
    {
        id = "terra boots",
        chance = 720,
    },
    {
        id = "great spirit potion",
        chance = 20000,
        maxCount = 3,
    },
    {
        id = "ultimate health potion",
        chance = 19000,
        maxCount = 3,
    },
    {
        id = "spellbook of mind control",
        chance = 720,
    },
    {
        id = "underworld rod",
        chance = 720,
    },
    {
        id = "springsprout rod",
        chance = 510,
    },
    {
        id = "green crystal shard",
        chance = 1540,
    },
    {
        id = "brown crystal splinter",
        chance = 52500,
        maxCount = 2,
    },
    {
        id = "blue crystal splinter",
        chance = 10000,
        maxCount = 3,
    },
    {
        id = "cluster of solace",
        chance = 620,
    },
    {
        id = "dead weight",
        chance = 14180,
    },
    {
        id = "hemp rope",
        chance = 14180,
    },
}

mtype:register(monster)
