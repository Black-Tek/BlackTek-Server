local mtype = Game.createMonsterType("Inky")
local monster = {}

monster.name = "Inky"
monster.description = "Inky"
monster.experience = 250
monster.race = "blood"
monster.maxHealth = 600
monster.health = 600
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -741,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        radius = 3,
        target = false,
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
        name = "physical",
        interval = 2000,
        chance = 7,
        range = 7,
        minDamage = -56,
        maxDamage = -87,
        target = true,
        shootEffect = CONST_ANI_SNOWBALL,
    },
    {
        name = "energycondition",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -75,
        maxDamage = -75,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
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
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Tssss!", yell = false},
    {text = "Gaaahhh!", yell = false},
    {text = "Gluh! Gluh!", yell = false},
    {text = "Boohaa!", yell = false},
}

mtype:register(monster)
