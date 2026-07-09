local mtype = Game.createMonsterType("Insectoid Worker")
local monster = {}

monster.name = "Insectoid Worker"
monster.description = "an insectoid worker"
monster.experience = 650
monster.race = "venom"
monster.maxHealth = 950
monster.health = 950
monster.speed = 200
monster.manaCost = 0
monster.corpse = 13514
monster.outfit = { lookType = 403 }
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
        maxDamage = -163,
        -- NOTE: melee condition (poison=160); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 160000,
        },
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 90,
    },
    {
        id = "small emerald",
        chance = 2880,
    },
    {
        id = "epee",
        chance = 560,
    },
    {
        id = "health potion",
        chance = 5090,
    },
    {
        id = "compound eye",
        chance = 15380,
    },
    {
        id = 15622,
        chance = 14990,
    },
}

mtype:register(monster)
