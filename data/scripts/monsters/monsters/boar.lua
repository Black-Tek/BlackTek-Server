local mtype = Game.createMonsterType("Boar")
local monster = {}

monster.name = "Boar"
monster.description = "a boar"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 198
monster.health = 198
monster.speed = 410
monster.manaCost = 465
monster.corpse = 13308
monster.outfit = { lookType = 380 }
monster.runHealth = 30
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -50,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Grunt! Grunt!", yell = false},
    {text = "Grunt", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 25313,
        maxCount = 20,
    },
    {
        id = 13297,
        chance = 19949,
        maxCount = 2,
    },
}

mtype:register(monster)
