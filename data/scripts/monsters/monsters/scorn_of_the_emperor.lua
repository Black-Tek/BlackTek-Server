local mtype = Game.createMonsterType("Scorn of the Emperor")
local monster = {}

monster.name = "Scorn of the Emperor"
monster.description = "Scorn of the Emperor"
monster.experience = 4600
monster.race = "undead"
monster.maxHealth = 7800
monster.health = 7800
monster.speed = 410
monster.manaCost = 0
monster.outfit = { lookType = 351 }
monster.runHealth = 366
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -374,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 17,
        minDamage = -150,
        maxDamage = -250,
        length = 8,
        spread = 3,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -500,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 10,
        range = 7,
        speed = -600,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 21,
        minDamage = -200,
        maxDamage = -450,
        radius = 6,
        target = false,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 35,
    armor = 45,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Draken Warmaster", interval = 2000, chance = 10, max = 2},
}

mtype:register(monster)
