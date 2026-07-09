local mtype = Game.createMonsterType("Deepling Scout")
local monster = {}

monster.name = "Deepling Scout"
monster.description = "a deepling scout"
monster.experience = 160
monster.race = "blood"
monster.maxHealth = 240
monster.health = 240
monster.speed = 200
monster.manaCost = 0
monster.corpse = 13839
monster.outfit = { lookType = 413 }
monster.runHealth = 50
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
        maxDamage = -100,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -40,
        maxDamage = -100,
        target = true,
        shootEffect = CONST_ANI_SPEAR,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Njaaarh!!", yell = false},
    {text = "Begjone, intrjuder!!", yell = false},
    {text = "Djon't djare stjare injo the eyes of the djeep!", yell = false},
    {text = "Ljeave this sjacred pljace while you cjan", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 50,
    },
    {
        id = "small emerald",
        chance = 121,
    },
    {
        id = "life ring",
        chance = 2127,
    },
    {
        id = "hunting spear",
        chance = 14285,
        maxCount = 3,
    },
    {
        id = "fish fin",
        chance = 310,
    },
    {
        id = 9809,
        chance = 925,
    },
    {
        id = "flask of rust remover",
        chance = 111,
    },
    {
        id = "heavy trident",
        chance = 505,
    },
    {
        id = "eye of a deepling",
        chance = 310,
    },
}

mtype:register(monster)
