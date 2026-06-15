local mtype = Game.createMonsterType("Tiquandas Revenge")
local monster = {}

monster.name = "Tiquandas Revenge"
monster.description = "Tiquandas Revenge"
monster.experience = 2635
monster.race = "venom"
monster.maxHealth = 1800
monster.health = 1800
monster.speed = 440
monster.manaCost = 0
monster.corpse = 6047
monster.outfit = { lookType = 120 }
monster.runHealth = 400
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -160,
        -- NOTE: melee condition (poison=95); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 95000,
        },
    },
    {
        name = "earth",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -60,
        maxDamage = -200,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 34,
        range = 7,
        speed = -850,
        duration = 30000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 12,
        minDamage = -40,
        maxDamage = -130,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 31,
    armor = 30,
    {
        name = "healing",
        interval = 1200,
        chance = 35,
        minDamage = 600,
        maxDamage = 800,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 5015,
        chance = 100000,
    },
    {
        id = 7732,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 87234,
        maxCount = 118,
    },
    {
        id = 2666,
        chance = 70213,
    },
    {
        id = 2671,
        chance = 38298,
    },
    {
        id = 2792,
        chance = 21277,
        maxCount = 5,
    },
    {
        id = 13298,
        chance = 12766,
    },
}

mtype:register(monster)
