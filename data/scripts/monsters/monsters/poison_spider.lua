local mtype = Game.createMonsterType("Poison Spider")
local monster = {}

monster.name = "Poison Spider"
monster.description = "a poison spider"
monster.experience = 22
monster.race = "venom"
monster.maxHealth = 26
monster.health = 26
monster.speed = 160
monster.manaCost = 270
monster.corpse = 5974
monster.outfit = { lookType = 36 }
monster.runHealth = 6
monster.changeTarget = {
    interval = 4000,
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
        maxDamage = -20,
        -- NOTE: melee condition (poison=30); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 30000,
        },
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 75000,
        maxCount = 4,
    },
    {
        id = "poison spider shell",
        chance = 1140,
    },
}

mtype:register(monster)
