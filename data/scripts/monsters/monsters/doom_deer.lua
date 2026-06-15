local mtype = Game.createMonsterType("Doom Deer")
local monster = {}

monster.name = "Doom Deer"
monster.description = "a doom deer"
monster.experience = 200
monster.race = "blood"
monster.maxHealth = 405
monster.health = 405
monster.speed = 300
monster.manaCost = 0
monster.corpse = 5970
monster.outfit = { lookType = 31 }
monster.runHealth = 25
monster.changeTarget = {
    interval = 2000,
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
    {
        name = "energy",
        interval = 4000,
        chance = 30,
        minDamage = -35,
        maxDamage = -55,
        length = 5,
        effect = CONST_ME_BIGCLOUDS,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
    {
        name = "speed",
        interval = 3000,
        chance = 30,
        speed = 400,
        duration = 8000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I bet it was you who killed my mom!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
}

mtype:register(monster)
