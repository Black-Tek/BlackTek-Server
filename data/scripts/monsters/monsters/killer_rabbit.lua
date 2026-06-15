local mtype = Game.createMonsterType("Killer Rabbit")
local monster = {}

monster.name = "Killer Rabbit"
monster.description = "a killer rabbit"
monster.experience = 160
monster.race = "blood"
monster.maxHealth = 205
monster.health = 205
monster.speed = 300
monster.manaCost = 220
monster.corpse = 6017
monster.outfit = { lookType = 74 }
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
        interval = 1200,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 30,
        range = 1,
        minDamage = 0,
        maxDamage = -50,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
    {
        name = "speed",
        interval = 1000,
        chance = 40,
        speed = 380,
        duration = 8000,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 30,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.maxSummons = 2
monster.summons = {
    {name = "killer rabbit", interval = 2000, chance = 30, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Who is lunch NOW?", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 90,
    },
}

mtype:register(monster)
