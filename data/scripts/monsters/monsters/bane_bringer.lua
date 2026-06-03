local mtype = Game.createMonsterType("Bane Bringer")
local monster = {}

monster.name = "Bane Bringer"
monster.description = "a bane bringer"
monster.experience = 400
monster.race = "undead"
monster.maxHealth = 2500
monster.health = 2500
monster.speed = 260
monster.manaCost = 0
monster.corpse = 9867
monster.outfit = { lookType = 310 }
monster.changeTarget = {
    interval = 4000,
    chance = 8,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
}
monster.defenses = {
    defense = 55,
    armor = 50,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -50},
    {type = COMBAT_DEATHDAMAGE, percent = -90},
    {type = COMBAT_FIREDAMAGE, percent = -80},
    {type = COMBAT_ENERGYDAMAGE, percent = -80},
    {type = COMBAT_ICEDAMAGE, percent = -80},
    {type = COMBAT_EARTHDAMAGE, percent = -80},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 30,
    {text = "You shall not succeed!", yell = false},
    {text = "This time we will prevail!", yell = false},
}
monster.loot = {
    {
        id = 2250,
        chance = 14285,
        maxCount = 96,
    },
    {
        id = "dry piece of wood",
        chance = 100000,
    },
}

mtype:register(monster)
