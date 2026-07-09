local mtype = Game.createMonsterType("Quara Predator Scout")
local monster = {}

monster.name = "Quara Predator Scout"
monster.description = "a quara predator scout"
monster.experience = 400
monster.race = "blood"
monster.maxHealth = 890
monster.health = 890
monster.speed = 170
monster.manaCost = 0
monster.corpse = 6067
monster.outfit = { lookType = 20 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -190,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
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
        id = 2148,
        chance = 98959,
        maxCount = 132,
    },
    {
        id = 12447,
        chance = 10105,
    },
    {
        id = 2483,
        chance = 8959,
    },
    {
        id = 2387,
        chance = 5785,
    },
    {
        id = 2670,
        chance = 5045,
    },
    {
        id = 2145,
        chance = 5020,
        maxCount = 2,
    },
    {
        id = 2377,
        chance = 3011,
    },
    {
        id = 5895,
        chance = 1067,
    },
    {
        id = 8911,
        chance = 947,
    },
}

mtype:register(monster)
