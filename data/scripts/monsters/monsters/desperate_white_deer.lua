local mtype = Game.createMonsterType("Desperate White Deer")
local monster = {}

monster.name = "Desperate White Deer"
monster.description = "a desperate white deer"
monster.experience = 35
monster.race = "blood"
monster.maxHealth = 255
monster.health = 55
monster.speed = 225
monster.manaCost = 0
monster.corpse = 13513
monster.outfit = { lookType = 400 }
monster.runHealth = 55
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 400,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "*bell*", yell = false},
    {text = "ROOOAAARR!!", yell = false},
    {text = "*sniff*", yell = false},
    {text = "*wheeze*", yell = false},
}
monster.loot = {
    {
        id = 13533,
        chance = 21286,
    },
    {
        id = 13534,
        chance = 20418,
    },
    {
        id = 2671,
        chance = 19595,
    },
}

mtype:register(monster)
