local mtype = Game.createMonsterType("Charged Energy Elemental")
local monster = {}

monster.name = "Charged Energy Elemental"
monster.description = "a charged energy elemental"
monster.experience = 450
monster.race = "energy"
monster.maxHealth = 500
monster.health = 500
monster.speed = 270
monster.manaCost = 0
monster.corpse = 8966
monster.outfit = { lookType = 293 }
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
        interval = 2000,
        chance = 20,
        range = 6,
        minDamage = -168,
        maxDamage = -100,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energycondition",
        interval = 1000,
        chance = 15,
        radius = 3,
        target = false,
        effect = CONST_ME_YELLOWENERGY,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
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
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 22,
    },
    {
        id = "flash arrow",
        chance = 6250,
        maxCount = 3,
    },
    {
        id = "energy soil",
        chance = 2063,
    },
}

mtype:register(monster)
