local mtype = Game.createMonsterType("Salamander")
local monster = {}

monster.name = "Salamander"
monster.description = "a salamander"
monster.experience = 25
monster.race = "blood"
monster.maxHealth = 70
monster.health = 70
monster.speed = 120
monster.manaCost = 0
monster.corpse = 19707
monster.outfit = { lookType = 529 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
        maxDamage = -10,
        -- NOTE: melee condition (poison=10); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 10000,
        },
    },
}
monster.defenses = {
    defense = 3,
    armor = 3,
    {
        name = "healing",
        interval = 6000,
        chance = 20,
        minDamage = 0,
        maxDamage = 4,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Chrrch!", yell = false},
    {text = "Chhh!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 71933,
        maxCount = 15,
    },
    {
        id = 19737,
        chance = 34928,
    },
    {
        id = 2544,
        chance = 7985,
        maxCount = 3,
    },
    {
        id = 2120,
        chance = 5077,
    },
    {
        id = 2398,
        chance = 3978,
    },
    {
        id = 2460,
        chance = 1976,
    },
    {
        id = 7618,
        chance = 1480,
    },
    {
        id = 2456,
        chance = 739,
    },
    {
        id = 2419,
        chance = 487,
    },
}

mtype:register(monster)
