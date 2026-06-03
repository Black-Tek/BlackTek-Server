local mtype = Game.createMonsterType("Jesse the Wicked")
local monster = {}

monster.name = "Jesse the Wicked"
monster.description = "Jesse the Wicked"
monster.experience = 180
monster.race = "blood"
monster.maxHealth = 280
monster.health = 280
monster.speed = 220
monster.manaCost = 0
monster.corpse = 20434
monster.outfit = {
    lookType = 134,
    lookHead = 95,
    lookBody = 95,
    lookLegs = 95,
    lookFeet = 94,
    lookAddons = 3,
}
monster.changeTarget = {
    interval = 4000,
    chance = 5,
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
        maxDamage = -100,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "That banker never saw it coming!", yell = false},
}
monster.loot = {
    {
        id = "bag with stolen gold",
        chance = 100000,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 85,
    },
}

mtype:register(monster)
