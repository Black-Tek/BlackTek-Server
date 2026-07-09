local mtype = Game.createMonsterType("The Frog Prince")
local monster = {}

monster.name = "The Frog Prince"
monster.description = "The Frog Prince"
monster.experience = 1
monster.race = "blood"
monster.maxHealth = 55
monster.health = 55
monster.speed = 320
monster.manaCost = 0
monster.corpse = 6079
monster.outfit = { lookType = 224 }
monster.changeTarget = {
    interval = 5000,
    chance = 20,
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
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1,
    },
}
monster.defenses = {
    defense = 2,
    armor = 3,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_ENERGYDAMAGE, percent = 90},
    {type = COMBAT_ICEDAMAGE, percent = 90},
    {type = COMBAT_FIREDAMAGE, percent = 90},
    {type = COMBAT_PHYSICALDAMAGE, percent = 98},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Don't Kill me!!!", yell = false},
    {text = "Have mercy!", yell = false},
    {text = "Perhaps a kiss might release me!", yell = false},
}

mtype:register(monster)
