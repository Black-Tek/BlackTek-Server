local mtype = Game.createMonsterType("Dragonling")
local monster = {}

monster.name = "Dragonling"
monster.description = "a dragonling"
monster.experience = 2200
monster.race = "blood"
monster.maxHealth = 2600
monster.health = 2600
monster.speed = 330
monster.manaCost = 0
monster.corpse = 18438
monster.outfit = { lookType = 505 }
monster.runHealth = 900
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -210,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -120,
        maxDamage = -250,
        length = 5,
        spread = 2,
        target = false,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 20,
        minDamage = -115,
        maxDamage = -180,
        radius = 3,
        target = false,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        minDamage = -95,
        maxDamage = -150,
        radius = 3,
        target = false,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 1,
        target = true,
        speed = -700,
        duration = 16000,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 100,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "FCHHHHHHHHHHHHHHHH", yell = false},
    {text = "FI?", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 37500,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 37500,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 37500,
        maxCount = 27,
    },
    {
        id = "strong health potion",
        chance = 24480,
        maxCount = 2,
    },
    {
        id = "strong mana potion",
        chance = 16900,
        maxCount = 2,
    },
    {
        id = "blazing bone",
        chance = 12410,
    },
}

mtype:register(monster)
