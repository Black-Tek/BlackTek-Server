local mtype = Game.createMonsterType("Thul")
local monster = {}

monster.name = "Thul"
monster.description = "Thul"
monster.experience = 2700
monster.race = "blood"
monster.maxHealth = 2950
monster.health = 2950
monster.speed = 520
monster.manaCost = 670
monster.corpse = 6065
monster.outfit = { lookType = 46 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1354,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 25,
        minDamage = 0,
        maxDamage = -170,
        radius = 3,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "poisonfield",
        interval = 2000,
        chance = 19,
        range = 7,
        radius = 3,
        target = false,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 18,
        range = 7,
        speed = -360,
        duration = 5000,
        shootEffect = CONST_ANI_SNOWBALL,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -108,
        maxDamage = -137,
        target = true,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_ICEAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Massive Water Elementals", interval = 2000, chance = 10, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gaaahhh!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 77,
    },
    {
        id = "shrimp",
        chance = 25000,
    },
    {
        id = "fish fin",
        chance = 100000,
    },
    {
        id = "relic sword",
        chance = 50000,
    },
}

mtype:register(monster)
