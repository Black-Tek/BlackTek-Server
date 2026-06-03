local mtype = Game.createMonsterType("Larva")
local monster = {}

monster.name = "Larva"
monster.description = "a larva"
monster.experience = 44
monster.race = "venom"
monster.maxHealth = 70
monster.health = 70
monster.speed = 124
monster.manaCost = 355
monster.corpse = 6023
monster.outfit = { lookType = 82 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -35,
        -- NOTE: melee condition (poison=15); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 15000,
        },
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 65000,
        maxCount = 15,
    },
    {
        id = "meat",
        chance = 15000,
    },
}

mtype:register(monster)
