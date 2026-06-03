local mtype = Game.createMonsterType("Skeleton")
local monster = {}

monster.name = "Skeleton"
monster.description = "a skeleton"
monster.experience = 35
monster.race = "undead"
monster.maxHealth = 50
monster.health = 50
monster.speed = 154
monster.manaCost = 300
monster.corpse = 5972
monster.outfit = { lookType = 33 }
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
        maxDamage = -17,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -7,
        maxDamage = -13,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
}
monster.loot = {
    {
        id = 2230,
        chance = 50915,
    },
    {
        id = 2148,
        chance = 44978,
        maxCount = 10,
    },
    {
        id = 2050,
        chance = 10285,
    },
    {
        id = 12437,
        chance = 10178,
    },
    {
        id = 2473,
        chance = 7687,
    },
    {
        id = 2388,
        chance = 5375,
    },
    {
        id = 2398,
        chance = 4625,
    },
    {
        id = 2376,
        chance = 2107,
    },
    {
        id = 2511,
        chance = 920,
    },
}

mtype:register(monster)
