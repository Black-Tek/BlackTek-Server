local mtype = Game.createMonsterType("Vicious Manbat")
local monster = {}

monster.name = "Vicious Manbat"
monster.description = "a vicious manbat"
monster.experience = 1200
monster.race = "blood"
monster.maxHealth = 1700
monster.health = 1700
monster.speed = 220
monster.manaCost = 0
monster.corpse = 21266
monster.outfit = { lookType = 554 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -215,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = -100,
        maxDamage = -250,
        radius = 3,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 1,
        target = true,
        speed = -400,
        effect = CONST_ME_BATS,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 20,
        minDamage = -400,
        maxDamage = -600,
        radius = 2,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
}

mtype:register(monster)
