local mtype = Game.createMonsterType("Gloom Wolf")
local monster = {}

monster.name = "Gloom Wolf"
monster.description = "a gloom wolf"
monster.experience = 70
monster.race = "blood"
monster.maxHealth = 200
monster.health = 200
monster.speed = 250
monster.manaCost = 0
monster.corpse = 24639
monster.outfit = { lookType = 716 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = true,
    canWalkOnFire = true,
    canWalkOnPoison = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -92,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 15,
        minDamage = -12,
        maxDamage = -26,
        radius = 2,
        target = false,
        effect = CONST_ME_HITAREA,
    },
}
monster.defenses = {
    defense = 8,
    armor = 10,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 250,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
}
monster.loot = {
    {
        id = 2148,
        chance = 66035,
        maxCount = 15,
    },
    {
        id = 2666,
        chance = 40928,
    },
    {
        id = 2671,
        chance = 39744,
    },
    {
        id = 24663,
        chance = 8100,
    },
    {
        id = 2129,
        chance = 7248,
    },
    {
        id = 5897,
        chance = 3221,
    },
}

mtype:register(monster)
