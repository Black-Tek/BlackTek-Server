local mtype = Game.createMonsterType("Thief")
local monster = {}

monster.name = "Thief"
monster.description = "a thief"
monster.experience = 5
monster.race = "blood"
monster.maxHealth = 60
monster.health = 60
monster.speed = 170
monster.manaCost = 0
monster.corpse = 20518
monster.outfit = {
    lookType = 134,
    lookHead = 114,
    lookBody = 97,
    lookLegs = 21,
    lookFeet = 41,
}
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -35,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Alarm!", yell = false},
    {text = "Don't let him escape!", yell = false},
    {text = "Intruder! Get him!", yell = false},
    {text = "You dare stealing from us?", yell = false},
}

mtype:register(monster)
