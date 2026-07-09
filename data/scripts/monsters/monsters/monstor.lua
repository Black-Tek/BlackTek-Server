local mtype = Game.createMonsterType("Monstor")
local monster = {}

monster.name = "Monstor"
monster.description = "Monstor"
monster.experience = 575
monster.race = "blood"
monster.maxHealth = 960
monster.health = 960
monster.speed = 350
monster.manaCost = 0
monster.corpse = 10322
monster.outfit = { lookType = 244 }
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -167,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 30,
        minDamage = -66,
        maxDamage = -85,
        length = 6,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 90,
        maxDamage = 200,
        effect = CONST_ME_HITBYFIRE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = -7},
    {type = COMBAT_HOLYDAMAGE, percent = -3},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Acid Blob", interval = 4000, chance = 40, max = 3},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "NO ARMY ME STOPPING! GRARR!", yell = false},
    {text = "ME DESTROY CITY! GROAR!", yell = false},
    {text = "WHARR! MUST ... KIDNAP WOMEN!", yell = false},
}

mtype:register(monster)
