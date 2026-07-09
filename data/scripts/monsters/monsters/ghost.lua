local mtype = Game.createMonsterType("Ghost")
local monster = {}

monster.name = "Ghost"
monster.description = "a ghost"
monster.experience = 120
monster.race = "undead"
monster.maxHealth = 150
monster.health = 150
monster.speed = 160
monster.manaCost = 0
monster.corpse = 5993
monster.outfit = { lookType = 48 }
monster.changeTarget = {
    interval = 4000,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -20,
        maxDamage = -45,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Huh!", yell = false},
    {text = "Shhhhhh", yell = false},
    {text = "Buuuuuh", yell = false},
}
monster.loot = {
    {
        id = 1962,
        chance = 1310,
    },
    {
        id = "stealth ring",
        chance = 180,
    },
    {
        id = "morning star",
        chance = 10610,
    },
    {
        id = "combat knife",
        chance = 7002,
    },
    {
        id = "ancient shield",
        chance = 860,
    },
    {
        id = "cape",
        chance = 8800,
    },
    {
        id = "shadow herb",
        chance = 14400,
    },
    {
        id = "white piece of cloth",
        chance = 1940,
    },
    {
        id = "ghostly tissue",
        chance = 1870,
    },
}

mtype:register(monster)
