local mtype = Game.createMonsterType("Green Frog")
local monster = {}

monster.name = "Green Frog"
monster.description = "a green frog"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 25
monster.health = 25
monster.speed = 200
monster.manaCost = 250
monster.corpse = 6079
monster.outfit = { lookType = 224 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 6
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
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
        maxDamage = -25,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ribbit!", yell = false},
    {text = "Ribbit! Ribbit!", yell = false},
}

mtype:register(monster)
