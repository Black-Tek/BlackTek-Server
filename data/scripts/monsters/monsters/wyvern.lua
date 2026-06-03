local mtype = Game.createMonsterType("Wyvern")
local monster = {}

monster.name = "Wyvern"
monster.description = "a wyvern"
monster.experience = 515
monster.race = "blood"
monster.maxHealth = 795
monster.health = 795
monster.speed = 186
monster.manaCost = 0
monster.corpse = 6302
monster.outfit = { lookType = 239 }
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
    illusionable = true,
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
        maxDamage = -120,
        -- NOTE: melee condition (poison=480); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 480000,
        },
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -240,
        maxDamage = -240,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        length = 3,
        spread = 2,
        duration = 5000,
        effect = CONST_ME_SOUND_RED,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 45,
        maxDamage = 65,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 5000,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Shriiiek", yell = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 99981,
        maxCount = 90,
    },
    {
        id = 2672,
        chance = 60229,
    },
    {
        id = 10561,
        chance = 11990,
    },
    {
        id = 2146,
        chance = 4949,
    },
    {
        id = 2547,
        chance = 3380,
    },
    {
        id = 7588,
        chance = 2473,
    },
    {
        id = 2187,
        chance = 784,
    },
    {
        id = 2127,
        chance = 590,
    },
    {
        id = 7408,
        chance = 459,
    },
}

mtype:register(monster)
