local mtype = Game.createMonsterType("Deepling Tyrant")
local monster = {}

monster.name = "Deepling Tyrant"
monster.description = "a deepling tyrant"
monster.experience = 4200
monster.race = "blood"
monster.maxHealth = 4900
monster.health = 4900
monster.speed = 260
monster.manaCost = 0
monster.corpse = 15188
monster.outfit = { lookType = 442 }
monster.runHealth = 20
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
    pushable = true,
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
        maxDamage = -501,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -375,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDSWORD,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -180,
        maxDamage = -215,
        target = true,
        shootEffect = CONST_ANI_SPEAR,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 200,
        maxDamage = 400,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "QJELL NETA NA!!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 97,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 97,
    },
    {
        id = "platinum coin",
        chance = 70000,
        maxCount = 4,
    },
    {
        id = "great mana potion",
        chance = 32640,
        maxCount = 3,
    },
    {
        id = "great health potion",
        chance = 33430,
        maxCount = 3,
    },
    {
        id = "eye of a deepling",
        chance = 28850,
    },
    {
        id = "deepling guard belt buckle",
        chance = 23100,
    },
    {
        id = "deepling breaktime snack",
        chance = 34770,
    },
    {
        id = "guardian axe",
        chance = 1420,
    },
    {
        id = "deepling claw",
        chance = 29960,
    },
    {
        id = "foxtail",
        chance = 80,
    },
    {
        id = "deepling backpack",
        chance = 510,
    },
    {
        id = "deepling squelcher",
        chance = 1540,
    },
}

mtype:register(monster)
