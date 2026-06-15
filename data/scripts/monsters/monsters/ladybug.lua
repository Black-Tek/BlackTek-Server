local mtype = Game.createMonsterType("Ladybug")
local monster = {}

monster.name = "Ladybug"
monster.description = "a ladybug"
monster.experience = 70
monster.race = "venom"
monster.maxHealth = 255
monster.health = 255
monster.speed = 200
monster.manaCost = 0
monster.corpse = 15272
monster.outfit = { lookType = 448 }
monster.runHealth = 60
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -4,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = 0,
        maxDamage = -20,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -20,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Nee pah!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 65000,
        maxCount = 40,
    },
}

mtype:register(monster)
