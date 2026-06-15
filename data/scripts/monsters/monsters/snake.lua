local mtype = Game.createMonsterType("Snake")
local monster = {}

monster.name = "Snake"
monster.description = "a snake"
monster.experience = 10
monster.race = "blood"
monster.maxHealth = 15
monster.health = 15
monster.speed = 120
monster.manaCost = 205
monster.corpse = 2817
monster.outfit = { lookType = 28 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -8,
        -- NOTE: melee condition (poison=15); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 15000,
        },
    },
}
monster.defenses = {
    defense = 0,
    armor = 0,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zzzzzzt", yell = false},
}

mtype:register(monster)
