local mtype = Game.createMonsterType("Kongra")
local monster = {}

monster.name = "Kongra"
monster.description = "a kongra"
monster.experience = 115
monster.race = "blood"
monster.maxHealth = 340
monster.health = 340
monster.speed = 184
monster.manaCost = 0
monster.corpse = 6043
monster.outfit = { lookType = 116 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 0,
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
        maxDamage = -60,
    },
}
monster.defenses = {
    defense = 18,
    armor = 18,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 260,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hugah!", yell = false},
    {text = "Ungh! Ungh!", yell = false},
    {text = "Huaauaauaauaa!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 88003,
        maxCount = 40,
    },
    {
        id = 2676,
        chance = 30813,
        maxCount = 12,
    },
    {
        id = 12427,
        chance = 9356,
    },
    {
        id = 2200,
        chance = 1012,
    },
    {
        id = 2463,
        chance = 1007,
    },
    {
        id = 5883,
        chance = 968,
    },
    {
        id = 7618,
        chance = 507,
    },
    {
        id = 2166,
        chance = 310,
    },
    {
        id = 2209,
        chance = 196,
    },
}

mtype:register(monster)
