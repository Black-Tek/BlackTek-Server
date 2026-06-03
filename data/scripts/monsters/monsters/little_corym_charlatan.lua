local mtype = Game.createMonsterType("Little Corym Charlatan")
local monster = {}

monster.name = "Little Corym Charlatan"
monster.description = "a little corym charlatan"
monster.experience = 40
monster.race = "blood"
monster.maxHealth = 90
monster.health = 90
monster.speed = 170
monster.manaCost = 0
monster.corpse = 19970
monster.outfit = {
    lookType = 532,
    lookHead = 0,
    lookBody = 79,
    lookLegs = 80,
    lookFeet = 0,
}
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
    illusionable = true,
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
        maxDamage = -14,
    },
}
monster.defenses = {
    defense = 3,
    armor = 3,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Squeeeeeeak!", yell = false},
    {text = "Must have it! Must have it!", yell = false},
    {text = "Gimme! Gimme!", yell = false},
}

mtype:register(monster)
