local mtype = Game.createMonsterType("Hive Overseer")
local monster = {}

monster.name = "Hive Overseer"
monster.description = "a hive overseer"
monster.experience = 5500
monster.race = "venom"
monster.maxHealth = 7500
monster.health = 7500
monster.speed = 200
monster.manaCost = 0
monster.corpse = 15354
monster.outfit = { lookType = 458 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -450,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        minDamage = -60,
        maxDamage = -80,
        radius = 4,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 20,
        minDamage = -600,
        maxDamage = -1000,
        radius = 4,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "healing",
        interval = 2000,
        chance = 50,
        minDamage = 50,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 500,
        maxDamage = 700,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 70},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 60},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Spidris Elite", interval = 2000, chance = 40, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zopp!", yell = false},
    {text = "Kropp!", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 16000,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 58000,
        maxCount = 98,
    },
    {
        id = "platinum coin",
        chance = 84000,
        maxCount = 6,
    },
    {
        id = "steel boots",
        chance = 550,
    },
    {
        id = "great mana potion",
        chance = 18000,
    },
    {
        id = 7632,
        chance = 6000,
    },
    {
        id = "ultimate health potion",
        chance = 12000,
    },
    {
        id = "gold ingot",
        chance = 29000,
    },
    {
        id = "kollos shell",
        chance = 28000,
    },
    {
        id = "compound eye",
        chance = 16000,
    },
    {
        id = "calopteryx cape",
        chance = 830,
    },
    {
        id = "carapace shield",
        chance = 920,
    },
    {
        id = "hive scythe",
        chance = 1650,
    },
    {
        id = "gooey mass",
        chance = 13000,
        maxCount = 2,
    },
    {
        id = "hive bow",
        chance = 830,
    },
}

mtype:register(monster)
