local mtype = Game.createMonsterType("The Bloodtusk")
local monster = {}

monster.name = "The Bloodtusk"
monster.description = "The Bloodtusk"
monster.experience = 300
monster.race = "blood"
monster.maxHealth = 600
monster.health = 600
monster.speed = 190
monster.manaCost = 0
monster.corpse = 6074
monster.outfit = { lookType = 199 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
    },
}
monster.defenses = {
    defense = 57,
    armor = 40,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 66900,
        maxCount = 65,
    },
    {
        id = "platinum coin",
        chance = 83825,
        maxCount = 3,
    },
    {
        id = "tusk shield",
        chance = 83825,
    },
    {
        id = "furry club",
        chance = 50500,
    },
    {
        id = "mammoth fur cape",
        chance = 42175,
    },
    {
        id = "orc tusk",
        chance = 58825,
        maxCount = 2,
    },
    {
        id = "mammoth tusk",
        chance = 38000,
        maxCount = 2,
    },
}

mtype:register(monster)
