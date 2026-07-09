local mtype = Game.createMonsterType("Swampling")
local monster = {}

monster.name = "Swampling"
monster.description = "a swampling"
monster.experience = 45
monster.race = "venom"
monster.maxHealth = 80
monster.health = 80
monster.speed = 190
monster.manaCost = 0
monster.corpse = 19902
monster.outfit = { lookType = 535 }
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -2,
        maxDamage = -15,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        length = 3,
        spread = 2,
        speed = -300,
        duration = 5000,
        effect = CONST_ME_WATERSPLASH,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gnark!", yell = false},
    {text = "Crrrck!", yell = false},
}
monster.loot = {
    {
        id = 2120,
        chance = 4540,
    },
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 12,
    },
    {
        id = "white mushroom",
        chance = 8980,
        maxCount = 5,
    },
    {
        id = "rat cheese",
        chance = 18670,
    },
    {
        id = "swampling moss",
        chance = 12610,
    },
    {
        id = "piece of swampling wood",
        chance = 8270,
    },
}

mtype:register(monster)
