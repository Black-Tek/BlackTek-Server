local mtype = Game.createMonsterType("Grave Guard")
local monster = {}

monster.name = "Grave Guard"
monster.description = "a grave guard"
monster.experience = 485
monster.race = "undead"
monster.maxHealth = 720
monster.health = 720
monster.speed = 200
monster.manaCost = 0
monster.corpse = 13975
monster.outfit = { lookType = 234 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -147,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 5,
        radius = 1,
        target = false,
        effect = CONST_ME_SOUND_BLUE,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 90},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 95445,
        maxCount = 129,
    },
    {
        id = 2747,
        chance = 69968,
    },
    {
        id = 7620,
        chance = 20318,
    },
    {
        id = 7618,
        chance = 19920,
    },
    {
        id = 2159,
        chance = 4802,
    },
    {
        id = 6300,
        chance = 2181,
    },
    {
        id = 2440,
        chance = 933,
    },
}

mtype:register(monster)
