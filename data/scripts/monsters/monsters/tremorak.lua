local mtype = Game.createMonsterType("Tremorak")
local monster = {}

monster.name = "Tremorak"
monster.description = "Tremorak"
monster.experience = 1300
monster.race = "undead"
monster.maxHealth = 10000
monster.health = 10000
monster.speed = 290
monster.manaCost = 0
monster.corpse = 8933
monster.outfit = { lookType = 285 }
monster.changeTarget = {
    interval = 2000,
    chance = 5,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -115,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 16,
        minDamage = 0,
        maxDamage = -255,
        radius = 7,
        target = false,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 16,
        minDamage = 0,
        maxDamage = -405,
        length = 8,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 16,
        range = 7,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 16,
        minDamage = 75,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 35},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -15},
    {type = COMBAT_ENERGYDAMAGE, percent = 85},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "STOMP STOMP!", yell = true},
}

mtype:register(monster)
