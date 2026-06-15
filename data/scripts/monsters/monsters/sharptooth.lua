local mtype = Game.createMonsterType("Sharptooth")
local monster = {}

monster.name = "Sharptooth"
monster.description = "Sharptooth"
monster.experience = 1600
monster.race = "blood"
monster.maxHealth = 2500
monster.health = 2500
monster.speed = 290
monster.manaCost = 0
monster.corpse = 6067
monster.outfit = { lookType = 20 }
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
    illusionable = true,
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
        maxDamage = -1000,
    },
}
monster.defenses = {
    defense = 29,
    armor = 20,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 210,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 12,
        minDamage = 200,
        maxDamage = 240,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = 80},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gnarrr!", yell = false},
    {text = "Tcharrr!", yell = false},
    {text = "Rrrah!", yell = false},
    {text = "Rraaar!", yell = false},
}
monster.loot = {
    {
        id = "fishbone",
        chance = 50000,
    },
}

mtype:register(monster)
