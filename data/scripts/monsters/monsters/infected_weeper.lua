local mtype = Game.createMonsterType("Infected Weeper")
local monster = {}

monster.name = "Infected Weeper"
monster.description = "an infected weeper"
monster.experience = 4800
monster.race = "fire"
monster.maxHealth = 6800
monster.health = 6800
monster.speed = 250
monster.manaCost = 0
monster.corpse = 17251
monster.outfit = { lookType = 489 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
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
        maxDamage = -280,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        minDamage = -250,
        maxDamage = -700,
        length = 5,
        spread = 3,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 30,
        minDamage = -80,
        maxDamage = -250,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        length = 5,
        spread = 3,
        target = false,
        speed = -800,
        duration = 30000,
        effect = CONST_ME_BLOCKHIT,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "Parasite", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Moooaaan!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 98,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 7,
    },
    {
        id = 8748,
        chance = 1460,
    },
}

mtype:register(monster)
