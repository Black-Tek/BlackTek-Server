local mtype = Game.createMonsterType("Terrified Elephant")
local monster = {}

monster.name = "Terrified Elephant"
monster.description = "a terrified elephant"
monster.experience = 160
monster.race = "blood"
monster.maxHealth = 320
monster.health = 320
monster.speed = 210
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
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
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
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 800,
        duration = 5000,
    },
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
        chance = 40845,
    },
    {
        id = 2671,
        chance = 29165,
    },
    {
        id = 3956,
        chance = 8611,
        maxCount = 2,
    },
    {
        id = 3973,
        chance = 99,
    },
}

mtype:register(monster)
