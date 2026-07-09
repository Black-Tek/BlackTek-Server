local mtype = Game.createMonsterType("Kollos")
local monster = {}

monster.name = "Kollos"
monster.description = "Kollos"
monster.experience = 2400
monster.race = "venom"
monster.maxHealth = 3800
monster.health = 3800
monster.speed = 200
monster.manaCost = 0
monster.corpse = 15354
monster.outfit = { lookType = 458 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -315,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -500,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_EXPLOSIONHIT,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zopp!", yell = false},
    {text = "Flzlzlzlzlzlz!", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 8180,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 49000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 97,
    },
    {
        id = "platinum coin",
        chance = 66000,
        maxCount = 3,
    },
    {
        id = "ring of healing",
        chance = 3270,
    },
    {
        id = "steel boots",
        chance = 130,
    },
    {
        id = "great mana potion",
        chance = 8950,
        maxCount = 4,
    },
    {
        id = 7632,
        chance = 2600,
    },
    {
        id = "ultimate health potion",
        chance = 4000,
        maxCount = 3,
    },
    {
        id = "gold ingot",
        chance = 5160,
    },
    {
        id = "kollos shell",
        chance = 15390,
    },
    {
        id = "compound eye",
        chance = 15720,
    },
    {
        id = "calopteryx cape",
        chance = 360,
    },
    {
        id = "carapace shield",
        chance = 310,
    },
    {
        id = "hive scythe",
        chance = 700,
    },
    {
        id = "buggy backpack",
        chance = 460,
    },
    {
        id = "tarsal arrow",
        chance = 10210,
        maxCount = 5,
    },
}

mtype:register(monster)
