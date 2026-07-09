local mtype = Game.createMonsterType("Baron Brute")
local monster = {}

monster.name = "Baron Brute"
monster.description = "Baron Brute"
monster.experience = 3000
monster.race = "blood"
monster.maxHealth = 5025
monster.health = 5025
monster.speed = 290
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
        maxDamage = -475,
    },
}
monster.defenses = {
    defense = 35,
    armor = 22,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 80},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 10,
    {text = "Mash'n!", yell = false},
}

mtype:register(monster)
