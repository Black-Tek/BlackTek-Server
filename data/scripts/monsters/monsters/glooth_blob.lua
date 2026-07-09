local mtype = Game.createMonsterType("Glooth Blob")
local monster = {}

monster.name = "Glooth Blob"
monster.description = "a glooth blob"
monster.experience = 700
monster.race = "venom"
monster.maxHealth = 750
monster.health = 750
monster.speed = 160
monster.manaCost = 0
monster.corpse = 23479
monster.outfit = { lookType = 614 }
monster.changeTarget = {
    interval = 2000,
    chance = 3,
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
        attack = 40,
        skill = 45,
        -- NOTE: melee condition (poison=280); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 280000,
        },
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 13,
        minDamage = -400,
        maxDamage = -480,
        radius = 4,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poison",
        interval = 2000,
        chance = 11,
        minDamage = -85,
        maxDamage = -180,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_ENVENOMEDARROW,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "poison",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -60,
        maxDamage = -105,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 13,
    armor = 11,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Glooth Blob", interval = 2000, chance = 6, max = 0},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 70,
    },
    {
        id = 23553,
        chance = 15410,
    },
    {
        id = 2146,
        chance = 3310,
        maxCount = 2,
    },
    {
        id = 9970,
        chance = 3420,
        maxCount = 2,
    },
    {
        id = 23554,
        chance = 400,
    },
    {
        id = 23550,
        chance = 510,
    },
    {
        id = 23549,
        chance = 580,
    },
    {
        id = 23551,
        chance = 560,
    },
}

mtype:register(monster)
