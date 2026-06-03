local mtype = Game.createMonsterType("Energy Overlord")
local monster = {}

monster.name = "Energy Overlord"
monster.description = "an Energy Overlord"
monster.experience = 2800
monster.race = "undead"
monster.maxHealth = 4000
monster.health = 4000
monster.speed = 290
monster.manaCost = 0
monster.corpse = 8966
monster.outfit = { lookType = 290 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -200,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 25,
        minDamage = 0,
        maxDamage = -800,
        length = 7,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 11,
        range = 3,
        minDamage = 0,
        maxDamage = -200,
        target = true,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 9,
        minDamage = -50,
        maxDamage = -200,
        radius = 5,
        target = false,
        effect = CONST_ME_BIGPLANTS,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
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
    {type = COMBAT_FIREDAMAGE, percent = -1},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 64,
    },
    {
        id = "platinum coin",
        chance = 25000,
        maxCount = 2,
    },
}

mtype:register(monster)
