local mtype = Game.createMonsterType("Bear")
local monster = {}

monster.name = "Bear"
monster.description = "a bear"
monster.experience = 23
monster.race = "blood"
monster.maxHealth = 80
monster.health = 80
monster.speed = 156
monster.manaCost = 300
monster.corpse = 5975
monster.outfit = { lookType = 16 }
monster.runHealth = 15
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
    defense = 6,
    armor = 6,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrr", yell = false},
    {text = "Groar", yell = false},
}
monster.loot = {
    {
        id = 2666,
        chance = 39964,
    },
    {
        id = 2671,
        chance = 19987,
    },
    {
        id = 5896,
        chance = 1973,
    },
    {
        id = 5902,
        chance = 480,
    },
}

mtype:register(monster)
