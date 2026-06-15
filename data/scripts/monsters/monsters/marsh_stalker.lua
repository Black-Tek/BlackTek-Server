local mtype = Game.createMonsterType("Marsh Stalker")
local monster = {}

monster.name = "Marsh Stalker"
monster.description = "a marsh stalker"
monster.experience = 50
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 210
monster.manaCost = 0
monster.corpse = 19708
monster.outfit = { lookType = 530 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 5000,
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
        maxDamage = -10,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -8,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_EARTHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Rrrawk! Rrrrawk!", yell = false},
}
monster.loot = {
    {
        id = 2120,
        chance = 4530,
    },
    {
        id = "gold coin",
        chance = 65000,
        maxCount = 23,
    },
    {
        id = "longsword",
        chance = 7180,
    },
    {
        id = "meat",
        chance = 20360,
        maxCount = 2,
    },
    {
        id = 2667,
        chance = 20290,
        maxCount = 2,
    },
    {
        id = "worm",
        chance = 15000,
        maxCount = 2,
    },
    {
        id = "seeds",
        chance = 120,
    },
    {
        id = 19741,
        chance = 7180,
    },
    {
        id = "marsh stalker beak",
        chance = 9300,
    },
}

mtype:register(monster)
