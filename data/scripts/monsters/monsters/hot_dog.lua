local mtype = Game.createMonsterType("Hot Dog")
local monster = {}

monster.name = "Hot Dog"
monster.description = "a hot dog"
monster.experience = 190
monster.race = "blood"
monster.maxHealth = 505
monster.health = 505
monster.speed = 300
monster.manaCost = 220
monster.corpse = 5971
monster.outfit = { lookType = 32 }
monster.changeTarget = {
    interval = 2000,
    chance = 0,
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
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -55,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 30,
        minDamage = -30,
        maxDamage = -60,
        length = 8,
        spread = 3,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 30,
        range = 7,
        minDamage = -50,
        maxDamage = -50,
        target = true,
        effect = CONST_ME_FIREATTACK,
    },
}
monster.defenses = {
    defense = 2,
    armor = 1,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Wuff Wuff", yell = false},
    {text = "Grrr Wuff", yell = false},
    {text = "Show me how good you are without some rolled newspaper!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 75,
    },
}

mtype:register(monster)
