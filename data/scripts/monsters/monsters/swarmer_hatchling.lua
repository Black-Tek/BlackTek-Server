local mtype = Game.createMonsterType("Swarmer Hatchling")
local monster = {}

monster.name = "Swarmer Hatchling"
monster.description = "a swarmer hatchling"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 5
monster.health = 5
monster.speed = 190
monster.manaCost = 0
monster.corpse = 15388
monster.outfit = { lookType = 460 }
monster.changeTarget = {
    interval = 2000,
    chance = 50,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -5,
    },
    {
        name = "drown",
        interval = 1000,
        chance = 11,
        range = 5,
        minDamage = -5,
        maxDamage = -10,
        target = true,
        shootEffect = CONST_ANI_SPEAR,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Flzlzlzlzlzlzlz?", yell = false},
}

mtype:register(monster)
