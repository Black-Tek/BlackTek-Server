local mtype = Game.createMonsterType("Chakoya Tribewarden")
local monster = {}

monster.name = "Chakoya Tribewarden"
monster.description = "a chakoya tribewarden"
monster.experience = 40
monster.race = "blood"
monster.maxHealth = 68
monster.health = 68
monster.speed = 200
monster.manaCost = 305
monster.corpse = 7320
monster.outfit = { lookType = 259 }
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
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
        maxDamage = -30,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 25},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Quisavu tukavi!", yell = false},
    {text = "Si siyoqua jamjam!", yell = false},
    {text = "Achuq! jinuma!", yell = false},
    {text = "Si ji jusipa!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 30,
    },
    {
        id = "short sword",
        chance = 4680,
    },
    {
        id = "bone shield",
        chance = 1000,
    },
    {
        id = 2667,
        chance = 20000,
    },
    {
        id = "northern pike",
        chance = 90,
    },
    {
        id = "rainbow trout",
        chance = 130,
    },
    {
        id = "green perch",
        chance = 190,
    },
    {
        id = "mammoth whopper",
        chance = 300,
    },
}

mtype:register(monster)
