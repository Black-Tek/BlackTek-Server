local mtype = Game.createMonsterType("The Axeorcist")
local monster = {}

monster.name = "The Axeorcist"
monster.description = "The Axeorcist"
monster.experience = 3000
monster.race = "blood"
monster.maxHealth = 5000
monster.health = 5000
monster.speed = 250
monster.manaCost = 0
monster.corpse = 5980
monster.outfit = { lookType = 8 }
monster.changeTarget = {
    interval = 2000,
    chance = 9,
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
        maxDamage = -275,
    },
}
monster.defenses = {
    defense = 12,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 9,
    {text = "DEESTRUCTIOON!", yell = true},
    {text = "Blood! Carnage! Muhahaha!", yell = true},
}

mtype:register(monster)
