local mtype = Game.createMonsterType("Mad Sheep")
local monster = {}

monster.name = "Mad Sheep"
monster.description = "a mad sheep"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 22
monster.health = 22
monster.speed = 272
monster.manaCost = 0
monster.corpse = 5991
monster.outfit = { lookType = 14 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1,
    },
}
monster.defenses = {
    defense = 1,
    armor = 1,
    {
        name = "healing",
        interval = 2000,
        chance = 50,
        minDamage = 5,
        maxDamage = 10,
        effect = CONST_ME_HEARTS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 292,
        duration = 5000,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -1},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Maeh", yell = false},
    {text = "Groar!", yell = false},
    {text = "Fchhhh", yell = false},
    {text = "Meow!", yell = false},
    {text = "Woof!", yell = false},
}

mtype:register(monster)
