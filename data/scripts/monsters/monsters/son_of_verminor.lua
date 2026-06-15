local mtype = Game.createMonsterType("Son of Verminor")
local monster = {}

monster.name = "Son of Verminor"
monster.description = "Son of Verminor"
monster.experience = 5900
monster.race = "venom"
monster.maxHealth = 8500
monster.health = 8500
monster.speed = 240
monster.manaCost = 0
monster.corpse = 1490
monster.outfit = { lookType = 19 }
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -473,
        -- NOTE: melee condition (poison=450); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 450000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -150,
        maxDamage = -200,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -350,
        maxDamage = -390,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -220,
        maxDamage = -270,
        radius = 3,
        target = false,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 250,
        maxDamage = 350,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blubb", yell = false},
}

mtype:register(monster)
