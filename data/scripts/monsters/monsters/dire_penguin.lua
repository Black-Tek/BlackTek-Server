local mtype = Game.createMonsterType("Dire Penguin")
local monster = {}

monster.name = "Dire Penguin"
monster.description = "a dire penguin"
monster.experience = 119
monster.race = "blood"
monster.maxHealth = 173
monster.health = 173
monster.speed = 200
monster.manaCost = 0
monster.corpse = 7334
monster.outfit = { lookType = 250 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
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
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -35,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SMALLSTONE,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 4,
        target = false,
        speed = -600,
        duration = 9000,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = 310,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 18000,
        maxCount = 10,
    },
    {
        id = "dragon hammer",
        chance = 200,
    },
    {
        id = 2667,
        chance = 8000,
        maxCount = 4,
    },
    {
        id = "rainbow trout",
        chance = 8000,
    },
    {
        id = "green perch",
        chance = 7000,
    },
}

mtype:register(monster)
