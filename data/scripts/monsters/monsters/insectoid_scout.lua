local mtype = Game.createMonsterType("Insectoid Scout")
local monster = {}

monster.name = "Insectoid Scout"
monster.description = "an insectoid scout"
monster.experience = 150
monster.race = "venom"
monster.maxHealth = 230
monster.health = 230
monster.speed = 190
monster.manaCost = 0
monster.corpse = 13514
monster.outfit = { lookType = 403 }
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -74,
        -- NOTE: melee condition (poison=60); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 60000,
        },
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Chrrr! Chrr!", yell = false},
    {text = "Klk! Klk!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 80000,
        maxCount = 80,
    },
    {
        id = "club ring",
        chance = 100,
    },
    {
        id = "ripper lance",
        chance = 850,
    },
    {
        id = "health potion",
        chance = 280,
    },
}

mtype:register(monster)
