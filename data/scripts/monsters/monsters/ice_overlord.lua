local mtype = Game.createMonsterType("Ice Overlord")
local monster = {}

monster.name = "Ice Overlord"
monster.description = "an Ice Overlord"
monster.experience = 2800
monster.race = "undead"
monster.maxHealth = 4000
monster.health = 4000
monster.speed = 390
monster.manaCost = 0
monster.corpse = 8965
monster.outfit = { lookType = 11 }
monster.changeTarget = {
    interval = 20000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
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
        maxDamage = -400,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 18,
        radius = 6,
        target = false,
        speed = -800,
        duration = 5000,
        effect = CONST_ME_ICETORNADO,
    },
    {
        name = "ice",
        interval = 1000,
        chance = 9,
        range = 7,
        minDamage = -50,
        maxDamage = -400,
        target = true,
        shootEffect = CONST_ANI_SMALLICE,
        effect = CONST_ME_ICEATTACK,
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
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 38,
    },
    {
        id = "platinum coin",
        chance = 50000,
        maxCount = 3,
    },
    {
        id = "flawless ice crystal",
        chance = 100000,
    },
}

mtype:register(monster)
