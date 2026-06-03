local mtype = Game.createMonsterType("The Snapper")
local monster = {}

monster.name = "The Snapper"
monster.description = "The Snapper"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 300
monster.health = 300
monster.speed = 240
monster.manaCost = 0
monster.corpse = 6046
monster.outfit = { lookType = 119 }
monster.runHealth = 30
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
        maxDamage = -60,
    },
}
monster.defenses = {
    defense = 5,
    armor = 13,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50675,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50675,
        maxCount = 94,
    },
    {
        id = "life ring",
        chance = 100000,
    },
    {
        id = "plate armor",
        chance = 35800,
    },
    {
        id = "knight armor",
        chance = 4025,
    },
    {
        id = "plate legs",
        chance = 44025,
    },
    {
        id = "crocodile boots",
        chance = 9900,
    },
    {
        id = "health potion",
        chance = 51100,
        maxCount = 5,
    },
}

mtype:register(monster)
