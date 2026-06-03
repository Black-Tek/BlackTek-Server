local mtype = Game.createMonsterType("Enraged White Deer")
local monster = {}

monster.name = "Enraged White Deer"
monster.description = "an enraged white deer"
monster.experience = 165
monster.race = "blood"
monster.maxHealth = 255
monster.health = 255
monster.speed = 210
monster.manaCost = 0
monster.corpse = 13513
monster.outfit = { lookType = 400 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
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
        maxDamage = -45,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 4000,
        chance = 15,
        minDamage = 10,
        maxDamage = 40,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "*wheeze*", yell = false},
    {text = "ROOOAAARR!!", yell = false},
    {text = "*sniff*", yell = false},
    {text = "*bell*", yell = false},
}
monster.loot = {
    {
        id = 13534,
        chance = 19968,
    },
    {
        id = 2671,
        chance = 19727,
    },
    {
        id = 13533,
        chance = 19647,
    },
}

mtype:register(monster)
