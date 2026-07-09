local mtype = Game.createMonsterType("Spidris")
local monster = {}

monster.name = "Spidris"
monster.description = "Spidris"
monster.experience = 2600
monster.race = "venom"
monster.maxHealth = 3700
monster.health = 3700
monster.speed = 260
monster.manaCost = 0
monster.corpse = 15296
monster.outfit = { lookType = 457 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -298,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -150,
        maxDamage = -310,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 450,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Eeeeeeyyyyh!", yell = false},
    {text = "Iiiiieeeeeh!", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 11900,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 45000,
        maxCount = 4,
    },
    {
        id = "violet gem",
        chance = 770,
    },
    {
        id = 6300,
        chance = 2700,
    },
    {
        id = "titan axe",
        chance = 920,
    },
    {
        id = "great mana potion",
        chance = 11500,
        maxCount = 2,
    },
    {
        id = 7632,
        chance = 1700,
    },
    {
        id = "ultimate health potion",
        chance = 6250,
        maxCount = 2,
    },
    {
        id = "spidris mandible",
        chance = 14960,
    },
    {
        id = "compound eye",
        chance = 12500,
    },
    {
        id = "calopteryx cape",
        chance = 370,
    },
    {
        id = "carapace shield",
        chance = 720,
    },
    {
        id = "hive scythe",
        chance = 690,
    },
}

mtype:register(monster)
