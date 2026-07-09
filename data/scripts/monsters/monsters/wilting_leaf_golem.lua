local mtype = Game.createMonsterType("Wilting Leaf Golem")
local monster = {}

monster.name = "Wilting Leaf Golem"
monster.description = "a wilting leaf golem"
monster.experience = 145
monster.race = "blood"
monster.maxHealth = 380
monster.health = 380
monster.speed = 145
monster.manaCost = 390
monster.corpse = 21434
monster.outfit = { lookType = 573 }
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
        maxDamage = -120,
        -- NOTE: melee condition (poison=300); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 300000,
        },
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -50,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -150,
        maxDamage = -200,
        radius = 4,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        radius = 3,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -1},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "*crackle*", yell = false},
    {text = "*swwwwishhhh*", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 97230,
        maxCount = 45,
    },
    {
        id = "small emerald",
        chance = 830,
    },
    {
        id = "white mushroom",
        chance = 4860,
        maxCount = 3,
    },
    {
        id = "swampling club",
        chance = 4200,
    },
    {
        id = "dowser",
        chance = 10690,
    },
    {
        id = "fir cone",
        chance = 15630,
    },
}

mtype:register(monster)
