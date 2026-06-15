local mtype = Game.createMonsterType("Versperoth")
local monster = {}

monster.name = "Versperoth"
monster.description = "Versperoth"
monster.experience = 20000
monster.race = "blood"
monster.maxHealth = 100000
monster.health = 100000
monster.speed = 0
monster.manaCost = 0
monster.outfit = { lookType = 295 }
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -650,
    },
    {
        name = "firefield",
        interval = 1200,
        chance = 30,
        range = 7,
        radius = 4,
        target = false,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 40,
        range = 5,
        minDamage = -700,
        maxDamage = -1615,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "melee",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -750,
        maxDamage = -950,
        radius = 3,
        target = false,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 55,
    armor = 55,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 90},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 7
monster.summons = {
    {name = "Minion of Vesperoth", interval = 2000, chance = 16, max = 4},
}
monster.voices = {
    interval = 2500,
    chance = 10,
    {text = "GrrroaR!", yell = false},
    {text = "GROWL!", yell = false},
    {text = "Waaaah!", yell = false},
}

mtype:register(monster)
