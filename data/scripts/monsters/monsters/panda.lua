local mtype = Game.createMonsterType("Panda")
local monster = {}

monster.name = "Panda"
monster.description = "a panda"
monster.experience = 23
monster.race = "blood"
monster.maxHealth = 80
monster.health = 80
monster.speed = 156
monster.manaCost = 300
monster.corpse = 6049
monster.outfit = { lookType = 123 }
monster.runHealth = 15
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
    convinceable = true,
    pushable = false,
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
        maxDamage = -20,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrr", yell = false},
    {text = "Groar", yell = false},
}
monster.loot = {
    {
        id = 2666,
        chance = 69144,
    },
    {
        id = 2671,
        chance = 36695,
    },
    {
        id = 12401,
        chance = 9423,
    },
}

mtype:register(monster)
