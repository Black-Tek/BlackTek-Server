local mtype = Game.createMonsterType("The Count")
local monster = {}

monster.name = "The Count"
monster.description = "The Count"
monster.experience = 450
monster.race = "undead"
monster.maxHealth = 1250
monster.health = 1250
monster.speed = 370
monster.manaCost = 0
monster.corpse = 8937
monster.outfit = { lookType = 287 }
monster.changeTarget = {
    interval = 5000,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -945,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 9,
        minDamage = 0,
        maxDamage = -300,
        radius = 4,
        target = false,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 100,
        maxDamage = 195,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 3000,
        chance = 30,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "Banshee", interval = 4000, chance = 50, max = 0},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 98,
    },
    {
        id = "war hammer",
        chance = 2300,
    },
    {
        id = "the ring of the count",
        chance = 100000,
    },
}

mtype:register(monster)
