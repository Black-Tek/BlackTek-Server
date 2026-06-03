local mtype = Game.createMonsterType("Lesser Swarmer")
local monster = {}

monster.name = "Lesser Swarmer"
monster.description = "a lesser swarmer"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 230
monster.health = 230
monster.speed = 180
monster.manaCost = 0
monster.corpse = 15388
monster.outfit = { lookType = 460 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -77,
        -- NOTE: melee condition (poison=60); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 60000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 5,
        minDamage = -15,
        maxDamage = -70,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}

mtype:register(monster)
