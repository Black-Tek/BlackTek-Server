local mtype = Game.createMonsterType("Warlord Ruzad")
local monster = {}

monster.name = "Warlord Ruzad"
monster.description = "Warlord Ruzad"
monster.experience = 1700
monster.race = "blood"
monster.maxHealth = 2500
monster.health = 2500
monster.speed = 270
monster.manaCost = 0
monster.corpse = 6008
monster.outfit = { lookType = 2 }
monster.changeTarget = {
    interval = 2000,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
}
monster.defenses = {
    defense = 35,
    armor = 32,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 80},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Orc Berserker", interval = 2000, chance = 30, max = 0},
}
monster.loot = {
    {
        id = 2478,
        chance = 50000,
    },
    {
        id = 2148,
        chance = 25000,
        maxCount = 54,
    },
    {
        id = 2428,
        chance = 25000,
    },
    {
        id = 2463,
        chance = 25000,
    },
    {
        id = 2377,
        chance = 25000,
    },
    {
        id = 2490,
        chance = 12500,
    },
    {
        id = 3965,
        chance = 12500,
    },
    {
        id = 2666,
        chance = 12500,
    },
    {
        id = 2647,
        chance = 12500,
    },
    {
        id = 2419,
        chance = 12500,
    },
}

mtype:register(monster)
