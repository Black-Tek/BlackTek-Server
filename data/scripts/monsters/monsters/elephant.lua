local mtype = Game.createMonsterType("Elephant")
local monster = {}

monster.name = "Elephant"
monster.description = "an elephant"
monster.experience = 160
monster.race = "blood"
monster.maxHealth = 320
monster.health = 320
monster.speed = 190
monster.manaCost = 500
monster.corpse = 6052
monster.outfit = { lookType = 211 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
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
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 25},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hooooot-Toooooot!", yell = false},
    {text = "Tooooot!", yell = false},
    {text = "Trooooot!", yell = false},
}
monster.loot = {
    {
        id = 2666,
        chance = 40494,
    },
    {
        id = 2671,
        chance = 29278,
    },
    {
        id = 3956,
        chance = 1089,
        maxCount = 2,
    },
    {
        id = 3973,
        chance = 151,
    },
}

mtype:register(monster)
