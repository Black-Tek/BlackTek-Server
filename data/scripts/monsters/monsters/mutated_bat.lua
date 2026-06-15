local mtype = Game.createMonsterType("Mutated Bat")
local monster = {}

monster.name = "Mutated Bat"
monster.description = "a mutated bat"
monster.experience = 615
monster.race = "blood"
monster.maxHealth = 900
monster.health = 900
monster.speed = 210
monster.manaCost = 0
monster.corpse = 9829
monster.outfit = { lookType = 307 }
monster.runHealth = 300
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -168,
        -- NOTE: melee condition (poison=120); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 120000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -70,
        maxDamage = -180,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 15,
        minDamage = -30,
        maxDamage = -90,
        radius = 6,
        target = false,
        effect = CONST_ME_SOUND_WHITE,
    },
    {
        name = "mutated bat curse",
        interval = 2000,
        chance = 10,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -190,
        maxDamage = -240,
        length = 4,
        spread = 3,
        target = false,
    },
}
monster.defenses = {
    defense = 19,
    armor = 19,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 80,
        maxDamage = 95,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Shriiiiiek", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 92495,
        maxCount = 130,
    },
    {
        id = 9808,
        chance = 12568,
    },
    {
        id = 2513,
        chance = 8554,
    },
    {
        id = 2425,
        chance = 6597,
    },
    {
        id = 10579,
        chance = 5205,
    },
    {
        id = 2800,
        chance = 4862,
    },
    {
        id = 5894,
        chance = 4761,
        maxCount = 2,
    },
    {
        id = 2167,
        chance = 1029,
    },
    {
        id = 2144,
        chance = 908,
        maxCount = 3,
    },
    {
        id = 9809,
        chance = 726,
    },
    {
        id = 2150,
        chance = 625,
        maxCount = 2,
    },
    {
        id = 10016,
        chance = 141,
    },
    {
        id = 2529,
        chance = 121,
    },
    {
        id = 7386,
        chance = 121,
    },
}

mtype:register(monster)
