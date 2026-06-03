local mtype = Game.createMonsterType("Slick Water Elemental")
local monster = {}

monster.name = "Slick Water Elemental"
monster.description = "a slick water elemental"
monster.experience = 450
monster.race = "undead"
monster.maxHealth = 550
monster.health = 550
monster.speed = 280
monster.manaCost = 0
monster.corpse = 8965
monster.outfit = { lookType = 286 }
monster.changeTarget = {
    interval = 20000,
    chance = 15,
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
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -175,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -130,
        target = true,
        shootEffect = CONST_ANI_EARTH,
    },
    {
        name = "ice",
        interval = 1000,
        chance = 10,
        range = 6,
        minDamage = 0,
        maxDamage = -220,
        target = true,
        shootEffect = CONST_ANI_SNOWBALL,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 18,
        range = 4,
        minDamage = 0,
        maxDamage = -103,
        target = true,
        shootEffect = CONST_ANI_SMALLICE,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 90,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 40},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blubb", yell = false},
    {text = "Splipsh Splash", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 22500,
        maxCount = 70,
    },
    {
        id = "gold coin",
        chance = 22500,
        maxCount = 60,
    },
    {
        id = "shiver arrow",
        chance = 2575,
        maxCount = 3,
    },
    {
        id = "iced soil",
        chance = 6000,
    },
}

mtype:register(monster)
