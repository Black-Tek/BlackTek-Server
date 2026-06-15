local mtype = Game.createMonsterType("Wisp")
local monster = {}

monster.name = "Wisp"
monster.description = "a wisp"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 115
monster.health = 115
monster.speed = 162
monster.manaCost = 0
monster.corpse = 6324
monster.outfit = { lookType = 294 }
monster.runHealth = 115
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 15
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -3,
        maxDamage = -7,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 200,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 15,
        maxDamage = 25,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 40},
    {type = COMBAT_EARTHDAMAGE, percent = 90},
    {type = COMBAT_PHYSICALDAMAGE, percent = 60},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Crackle!", yell = false},
    {text = "Tsshh", yell = false},
}
monster.loot = {
    {
        id = "moon backpack",
        chance = 140,
    },
}

mtype:register(monster)
