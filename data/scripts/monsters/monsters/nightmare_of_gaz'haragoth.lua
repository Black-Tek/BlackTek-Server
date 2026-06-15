local mtype = Game.createMonsterType("Nightmare Of Gaz'haragoth")
local monster = {}

monster.name = "Nightmare Of Gaz'haragoth"
monster.description = "a nightmare of Gaz'haragoth"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 5500
monster.health = 5500
monster.speed = 360
monster.manaCost = 0
monster.outfit = { lookType = 587 }
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
        maxDamage = -300,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -500,
        maxDamage = -800,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 3000,
        chance = 17,
        minDamage = -500,
        maxDamage = -1000,
        length = 8,
        effect = CONST_ME_YELLOWENERGY,
    },
}
monster.defenses = {
    defense = 80,
    armor = 80,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
