local mtype = Game.createMonsterType("The Dark Dancer")
local monster = {}

monster.name = "The Dark Dancer"
monster.description = "The Dark Dancer"
monster.experience = 435
monster.race = "blood"
monster.maxHealth = 805
monster.health = 805
monster.speed = 170
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 58 }
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -45,
        -- NOTE: melee condition (poison=220); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 220000,
        },
    },
    {
        name = "physical",
        interval = 3000,
        chance = 70,
        range = 5,
        minDamage = -60,
        maxDamage = -95,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "physical",
        interval = 6000,
        chance = 90,
        range = 5,
        minDamage = -60,
        maxDamage = -95,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "speed",
        interval = 3500,
        chance = 35,
        range = 5,
        radius = 1,
        target = true,
        speed = -400,
        duration = 10000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 4000,
        chance = 30,
        range = 5,
        minDamage = -2,
        maxDamage = -110,
        radius = 1,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 12,
    armor = 11,
    {
        name = "healing",
        interval = 2000,
        chance = 45,
        minDamage = 75,
        maxDamage = 135,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 3000,
        chance = 50,
        duration = 5000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 40},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Ghoul", interval = 2000, chance = 20, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 5,
    {text = "I hope you like my voice!", yell = false},
}

mtype:register(monster)
