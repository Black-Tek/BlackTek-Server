local mtype = Game.createMonsterType("Crustacea Gigantica")
local monster = {}

monster.name = "Crustacea Gigantica"
monster.description = "a crustacea gigantica"
monster.experience = 1800
monster.race = "blood"
monster.maxHealth = 1600
monster.health = 1600
monster.speed = 290
monster.manaCost = 0
monster.corpse = 13331
monster.outfit = { lookType = 383 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -600,
    },
}
monster.defenses = {
    defense = 45,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 12},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Chrchrchr", yell = true},
    {text = "Klonklonk", yell = true},
    {text = "Chrrrrr", yell = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 43,
    },
    {
        id = "strong mana potion",
        chance = 14285,
    },
    {
        id = "giant crab pincer",
        chance = 5000,
    },
}

mtype:register(monster)
