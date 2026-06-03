local mtype = Game.createMonsterType("Midnight Spawn")
local monster = {}

monster.name = "Midnight Spawn"
monster.description = "a midnight spawn"
monster.experience = 900
monster.race = "undead"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 230
monster.manaCost = 0
monster.corpse = 9960
monster.outfit = { lookType = 315 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = 99},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 99},
    {type = COMBAT_LIFEDRAIN, percent = 99},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "midnight shard",
        chance = 8333,
    },
}

mtype:register(monster)
