local mtype = Game.createMonsterType("The Fettered Shatterer")
local monster = {}

monster.name = "The Fettered Shatterer"
monster.description = "The Fettered Shatterer"
monster.experience = 58000
monster.race = "fire"
monster.maxHealth = 220000
monster.health = 220000
monster.speed = 320
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 842,
    lookHead = 77,
    lookBody = 132,
    lookLegs = 20,
    lookFeet = 0,
    lookAddons = 0,
}
monster.runHealth = 2500
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = true,
    canWalkOnFire = true,
    canWalkOnPoison = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = -200,
        maxDamage = -3000,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -200,
        maxDamage = -1000,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 44,
        range = 7,
        minDamage = -400,
        maxDamage = -2000,
        shootEffect = CONST_ANI_WHIRLWINDSWORD,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -400,
        duration = 15000,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -800,
        radius = 7,
        target = false,
        effect = CONST_ME_BIGPLANTS,
    },
}
monster.defenses = {
    defense = 65,
    armor = 55,
    {
        name = "healing",
        interval = 3000,
        chance = 35,
        minDamage = 400,
        maxDamage = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 4000,
        chance = 80,
        speed = 440,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 30,
    {text = "RAAAAR! I! WILL! SHATTER! ALL!!", yell = false},
    {text = "WANT! TO! BREAK! FREE!!", yell = false},
    {text = "CRASH! BOOM! BANG! AHAHAHAAH!", yell = false},
    {text = "I WILL BREAK THE WORLD LIKE GLASS!", yell = false},
}

mtype:register(monster)
