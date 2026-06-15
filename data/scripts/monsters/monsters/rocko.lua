local mtype = Game.createMonsterType("Rocko")
local monster = {}

monster.name = "Rocko"
monster.description = "Rocko"
monster.experience = 3400
monster.race = "blood"
monster.maxHealth = 10000
monster.health = 10000
monster.speed = 180
monster.manaCost = 590
monster.corpse = 6005
monster.outfit = { lookType = 67 }
monster.changeTarget = {
    interval = 2000,
    chance = 9,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -190,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = -125,
        length = 8,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = -411,
        length = 8,
        effect = CONST_ME_GROUNDSHAKER,
    },
}
monster.defenses = {
    defense = 18,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_ICEDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
