local mtype = Game.createMonsterType("Pirate Skeleton")
local monster = {}

monster.name = "Pirate Skeleton"
monster.description = "a pirate skeleton"
monster.experience = 85
monster.race = "undead"
monster.maxHealth = 190
monster.health = 190
monster.speed = 180
monster.manaCost = 0
monster.corpse = 6070
monster.outfit = { lookType = 195 }
monster.runHealth = 20
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
    convinceable = false,
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
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 48000,
        maxCount = 25,
    },
    {
        id = 2229,
        chance = 4460,
    },
    {
        id = 2230,
        chance = 4250,
    },
    {
        id = 2231,
        chance = 5140,
    },
    {
        id = 2376,
        chance = 550,
    },
    {
        id = "short sword",
        chance = 1003,
    },
    {
        id = "bone club",
        chance = 960,
    },
    {
        id = "spooky blue eye",
        chance = 4730,
    },
}

mtype:register(monster)
