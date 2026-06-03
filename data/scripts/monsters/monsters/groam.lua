local mtype = Game.createMonsterType("Groam")
local monster = {}

monster.name = "Groam"
monster.description = "Groam"
monster.experience = 180
monster.race = "blood"
monster.maxHealth = 400
monster.health = 400
monster.speed = 560
monster.manaCost = 0
monster.corpse = 13839
monster.outfit = { lookType = 413 }
monster.runHealth = 60
monster.changeTarget = {
    interval = 2000,
    chance = 50,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -99,
    },
    {
        name = "drown",
        interval = 1000,
        chance = 11,
        range = 5,
        minDamage = -15,
        maxDamage = -100,
        target = true,
        shootEffect = CONST_ANI_SPEAR,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 20,
    armor = 12,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -20},
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
    {type = COMBAT_DEATHDAMAGE, percent = -20},
}
monster.immunities = {
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
        chance = 100000,
        maxCount = 50,
    },
    {
        id = "life ring",
        chance = 50000,
    },
    {
        id = "hunting spear",
        chance = 25000,
        maxCount = 4,
    },
    {
        id = 9808,
        chance = 14285,
    },
}

mtype:register(monster)
