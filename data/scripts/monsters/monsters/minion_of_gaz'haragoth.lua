local mtype = Game.createMonsterType("Minion Of Gaz'haragoth")
local monster = {}

monster.name = "Minion Of Gaz'haragoth"
monster.description = "a minion of Gaz'haragoth"
monster.experience = 0
monster.race = "fire"
monster.maxHealth = 5500
monster.health = 5500
monster.speed = 296
monster.manaCost = 0
monster.outfit = { lookType = 590 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -700,
    },
    {
        name = "energy",
        interval = 1800,
        chance = 10,
        minDamage = -300,
        maxDamage = -450,
        length = 7,
        effect = CONST_ME_STONES,
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -155,
        maxDamage = -220,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -100},
    {type = COMBAT_ICEDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
