local mtype = Game.createMonsterType("Draken Abomination")
local monster = {}

monster.name = "Draken Abomination"
monster.description = "a draken abomination"
monster.experience = 3800
monster.race = "venom"
monster.maxHealth = 6250
monster.health = 6250
monster.speed = 230
monster.manaCost = 0
monster.corpse = 12623
monster.outfit = { lookType = 357 }
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -420,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -310,
        maxDamage = -630,
        length = 4,
        spread = 3,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "draken abomination curse",
        interval = 2000,
        chance = 10,
        range = 5,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -170,
        maxDamage = -370,
        length = 4,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 15,
        range = 7,
        radius = 4,
        target = false,
        duration = 9000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 650,
        maxDamage = 700,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Death Blob", interval = 2000, chance = 10, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ugggh!", yell = false},
    {text = "Gll", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 47000,
        maxCount = 98,
    },
    {
        id = "platinum coin",
        chance = 50590,
        maxCount = 8,
    },
    {
        id = "meat",
        chance = 50450,
        maxCount = 4,
    },
    {
        id = "great mana potion",
        chance = 9950,
        maxCount = 3,
    },
    {
        id = "terra hood",
        chance = 8730,
    },
    {
        id = "great spirit potion",
        chance = 4905,
        maxCount = 3,
    },
    {
        id = "ultimate health potion",
        chance = 9400,
        maxCount = 3,
    },
    {
        id = "wand of voodoo",
        chance = 1020,
    },
    {
        id = "small topaz",
        chance = 2900,
        maxCount = 4,
    },
    {
        id = "Zaoan armor",
        chance = 470,
    },
    {
        id = "Zaoan helmet",
        chance = 560,
    },
    {
        id = "Zaoan legs",
        chance = 780,
    },
    {
        id = "eye of corruption",
        chance = 12110,
    },
    {
        id = "tail of corruption",
        chance = 6240,
    },
    {
        id = "scale of corruption",
        chance = 10940,
    },
    {
        id = "shield of corruption",
        chance = 10,
    },
    {
        id = "draken boots",
        chance = 540,
    },
    {
        id = "snake god's wristguard",
        chance = 10,
    },
    {
        id = "bamboo leaves",
        chance = 360,
    },
}

mtype:register(monster)
