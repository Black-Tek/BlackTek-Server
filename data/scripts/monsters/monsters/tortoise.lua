local mtype = Game.createMonsterType("Tortoise")
local monster = {}

monster.name = "Tortoise"
monster.description = "a tortoise"
monster.experience = 90
monster.race = "blood"
monster.maxHealth = 185
monster.health = 185
monster.speed = 200
monster.manaCost = 445
monster.corpse = 6072
monster.outfit = { lookType = 197 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = true,
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
        maxDamage = -50,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 35},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 59000,
        maxCount = 30,
    },
    {
        id = "battle hammer",
        chance = 730,
    },
    {
        id = "plate shield",
        chance = 2850,
    },
    {
        id = 2667,
        chance = 4600,
    },
    {
        id = "tortoise egg",
        chance = 770,
        maxCount = 2,
    },
    {
        id = "turtle shell",
        chance = 1300,
    },
    {
        id = "tortoise shield",
        chance = 200,
    },
}

mtype:register(monster)
