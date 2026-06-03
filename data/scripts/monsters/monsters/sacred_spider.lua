local mtype = Game.createMonsterType("Sacred Spider")
local monster = {}

monster.name = "Sacred Spider"
monster.description = "a sacred spider"
monster.experience = 330
monster.race = "venom"
monster.maxHealth = 550
monster.health = 550
monster.speed = 210
monster.manaCost = 485
monster.corpse = 6060
monster.outfit = { lookType = 219 }
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -160,
        -- NOTE: melee condition (poison=80); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 80000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -50,
        maxDamage = -80,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 19,
    },
    {
        id = "scarab coin",
        chance = 7142,
        maxCount = 4,
    },
    {
        id = "plate armor",
        chance = 4761,
    },
    {
        id = "spider fangs",
        chance = 25000,
    },
    {
        id = "gold ingot",
        chance = 95,
    },
}

mtype:register(monster)
