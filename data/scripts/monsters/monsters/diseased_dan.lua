local mtype = Game.createMonsterType("Diseased Dan")
local monster = {}

monster.name = "Diseased Dan"
monster.description = "a diseased Dan"
monster.experience = 300
monster.race = "venom"
monster.maxHealth = 800
monster.health = 800
monster.speed = 250
monster.manaCost = 0
monster.corpse = 8951
monster.outfit = { lookType = 299 }
monster.runHealth = 1
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
        -- NOTE: melee condition (poison=40); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 40000,
        },
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -31,
        maxDamage = -31,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -250,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 3000,
        chance = 40,
        target = true,
        speed = -900,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 5000,
        chance = 60,
        minDamage = 50,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 85},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Where... Where am I?", yell = false},
    {text = "Is that you, Tom?", yell = false},
    {text = "Phew, what an awful smell ... oh, that's me.", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 42000,
        maxCount = 25,
    },
}

mtype:register(monster)
