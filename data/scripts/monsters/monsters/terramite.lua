local mtype = Game.createMonsterType("Terramite")
local monster = {}

monster.name = "Terramite"
monster.description = "a terramite"
monster.experience = 160
monster.race = "venom"
monster.maxHealth = 365
monster.health = 365
monster.speed = 200
monster.manaCost = 505
monster.corpse = 11347
monster.outfit = { lookType = 346 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = true,
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
        maxDamage = -100,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -5,
        maxDamage = -16,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zrp zrp!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 97520,
        maxCount = 45,
    },
    {
        id = "terramite shell",
        chance = 7730,
    },
    {
        id = "terramite eggs",
        chance = 4680,
        maxCount = 3,
    },
    {
        id = "terramite legs",
        chance = 14880,
    },
}

mtype:register(monster)
