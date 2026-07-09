local mtype = Game.createMonsterType("Fire Overlord")
local monster = {}

monster.name = "Fire Overlord"
monster.description = "a Fire Overlord"
monster.experience = 2800
monster.race = "fire"
monster.maxHealth = 4000
monster.health = 4000
monster.speed = 300
monster.manaCost = 0
monster.corpse = 8964
monster.outfit = { lookType = 243 }
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -450,
        -- NOTE: melee condition (fire=650); verify damage/duration
        condition = {
            type = CONDITION_FIRE,
            interval = 650000,
        },
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 15,
        range = 7,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 15,
        minDamage = -300,
        maxDamage = -900,
        length = 1,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 13,
        minDamage = -200,
        maxDamage = -350,
        radius = 4,
        target = true,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 1},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_ICEDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 75,
    },
    {
        id = "platinum coin",
        chance = 50000,
        maxCount = 3,
    },
    {
        id = "magma coat",
        chance = 819,
    },
    {
        id = "eternal flames",
        chance = 100000,
    },
    {
        id = "fiery heart",
        chance = 100000,
    },
}

mtype:register(monster)
