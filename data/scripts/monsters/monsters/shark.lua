local mtype = Game.createMonsterType("Shark")
local monster = {}

monster.name = "Shark"
monster.description = "a shark"
monster.experience = 700
monster.race = "blood"
monster.maxHealth = 1200
monster.health = 1200
monster.speed = 230
monster.manaCost = 0
monster.corpse = 15287
monster.outfit = { lookType = 453 }
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
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -175,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 1},
    {type = COMBAT_FIREDAMAGE, percent = 1},
    {type = COMBAT_ICEDAMAGE, percent = 1},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Rarr chomp chomp!", yell = false},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 1222,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 38,
    },
    {
        id = 2667,
        chance = 25000,
        maxCount = 4,
    },
    {
        id = "fish fin",
        chance = 161,
    },
    {
        id = 7632,
        chance = 550,
    },
    {
        id = "eye of a deepling",
        chance = 1270,
    },
    {
        id = "deepling scales",
        chance = 9090,
    },
}

mtype:register(monster)
