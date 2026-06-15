local mtype = Game.createMonsterType("High Templar Cobrass")
local monster = {}

monster.name = "High Templar Cobrass"
monster.description = "High Templar Cobrass"
monster.experience = 515
monster.race = "blood"
monster.maxHealth = 410
monster.health = 410
monster.speed = 174
monster.manaCost = 0
monster.corpse = 4251
monster.outfit = { lookType = 113 }
monster.changeTarget = {
    interval = 5000,
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
}
monster.defenses = {
    defense = 26,
    armor = 26,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Hissss!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "lizard scale",
        chance = 100000,
    },
    {
        id = "lizard leather",
        chance = 100000,
    },
    {
        id = "short sword",
        chance = 10000,
    },
    {
        id = 2376,
        chance = 5200,
    },
    {
        id = "steel helmet",
        chance = 2500,
    },
    {
        id = "morning star",
        chance = 2490,
    },
    {
        id = "plate armor",
        chance = 1500,
    },
    {
        id = "health potion",
        chance = 1320,
    },
    {
        id = "templar scytheblade",
        chance = 1050,
    },
    {
        id = "small emerald",
        chance = 760,
    },
    {
        id = "salamander shield",
        chance = 610,
    },
}

mtype:register(monster)
