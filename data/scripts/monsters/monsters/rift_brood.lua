local mtype = Game.createMonsterType("Rift Brood")
local monster = {}

monster.name = "Rift Brood"
monster.description = "a rift brood"
monster.experience = 1600
monster.race = "energy"
monster.maxHealth = 2950
monster.health = 2950
monster.speed = 300
monster.manaCost = 0
monster.corpse = 1495
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
        maxDamage = -255,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 50,
        range = 1,
        minDamage = -100,
        maxDamage = -180,
        target = true,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 90,
        range = 3,
        minDamage = -200,
        maxDamage = -400,
        radius = 2,
        target = true,
        effect = CONST_ME_PURPLEENERGY,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 70,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
