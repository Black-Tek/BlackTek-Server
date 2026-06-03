local mtype = Game.createMonsterType("Wild Warrior")
local monster = {}

monster.name = "Wild Warrior"
monster.description = "a wild warrior"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 135
monster.health = 135
monster.speed = 190
monster.manaCost = 420
monster.corpse = 20531
monster.outfit = {
    lookType = 131,
    lookHead = 38,
    lookBody = 38,
    lookLegs = 38,
    lookFeet = 38,
}
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
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
        maxDamage = -70,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 200,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "An enemy!", yell = false},
    {text = "Gimme your money!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 65608,
        maxCount = 30,
    },
    {
        id = 2386,
        chance = 20940,
    },
    {
        id = 2511,
        chance = 17483,
    },
    {
        id = 2398,
        chance = 13081,
    },
    {
        id = 3606,
        chance = 12596,
        maxCount = 2,
    },
    {
        id = 2458,
        chance = 5204,
    },
    {
        id = 2465,
        chance = 2779,
    },
    {
        id = 2509,
        chance = 1314,
    },
    {
        id = 2459,
        chance = 1032,
    },
    {
        id = 2110,
        chance = 529,
    },
}

mtype:register(monster)
