local mtype = Game.createMonsterType("Insect Swarm")
local monster = {}

monster.name = "Insect Swarm"
monster.description = "an insect swarm"
monster.experience = 40
monster.race = "undead"
monster.maxHealth = 50
monster.health = 50
monster.speed = 150
monster.manaCost = 0
monster.corpse = 11320
monster.outfit = { lookType = 349 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -10,
        -- NOTE: melee condition (poison=16); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 16000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = 0,
        maxDamage = -15,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}

mtype:register(monster)
