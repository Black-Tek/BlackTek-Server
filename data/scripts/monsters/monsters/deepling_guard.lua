local mtype = Game.createMonsterType("Deepling Guard")
local monster = {}

monster.name = "Deepling Guard"
monster.description = "a deepling guard"
monster.experience = 2100
monster.race = "blood"
monster.maxHealth = 1900
monster.health = 1900
monster.speed = 220
monster.manaCost = 0
monster.corpse = 15187
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -400,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -200,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDSWORD,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_SPEAR,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 100,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
    {type = COMBAT_EARTHDAMAGE, percent = -20},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "QJELL NETA NA!!", yell = false},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 2890,
        maxCount = 3,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 90,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 90,
    },
    {
        id = "platinum coin",
        chance = 70000,
        maxCount = 2,
    },
    {
        id = "great mana potion",
        chance = 14285,
        maxCount = 3,
    },
    {
        id = "great health potion",
        chance = 14285,
        maxCount = 3,
    },
    {
        id = "heavy trident",
        chance = 1694,
    },
    {
        id = "eye of a deepling",
        chance = 10000,
    },
    {
        id = "deepling guard belt buckle",
        chance = 12500,
    },
    {
        id = "deepling breaktime snack",
        chance = 16666,
    },
    {
        id = "guardian axe",
        chance = 925,
    },
    {
        id = "deepling claw",
        chance = 9090,
    },
    {
        id = "foxtail",
        chance = 10,
    },
    {
        id = "ornate crossbow",
        chance = 362,
    },
    {
        id = "deepling backpack",
        chance = 333,
    },
    {
        id = "deepling squelcher",
        chance = 751,
    },
}

mtype:register(monster)
