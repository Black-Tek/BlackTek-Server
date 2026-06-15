local mtype = Game.createMonsterType("Grandfather Tridian")
local monster = {}

monster.name = "Grandfather Tridian"
monster.description = "Grandfather Tridian"
monster.experience = 1400
monster.race = "blood"
monster.maxHealth = 1800
monster.health = 1800
monster.speed = 210
monster.manaCost = 0
monster.corpse = 20391
monster.outfit = { lookType = 193 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 25,
        range = 1,
        minDamage = -138,
        maxDamage = -362,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_HOLY,
        effect = CONST_ME_HOLYAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = 0,
        maxDamage = -80,
        radius = 1,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 60,
        maxDamage = 90,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 4000,
        effect = CONST_ME_YELLOW_RINGS,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_PHYSICALDAMAGE, percent = 35},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "crypt shambler", interval = 2000, chance = 10, max = 0},
    {name = "ghost", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 2000,
    chance = 7,
    {text = "I will bring peace to your misguided soul!", yell = false},
    {text = "Your intrusion can't be tolerated!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 80,
    },
    {
        id = "piggy bank",
        chance = 100000,
    },
    {
        id = "strong mana potion",
        chance = 5000,
    },
    {
        id = "brown mushroom",
        chance = 5000,
    },
    {
        id = "wand of inferno",
        chance = 5000,
    },
    {
        id = "skull staff",
        chance = 5000,
    },
    {
        id = "wand of voodoo",
        chance = 5000,
    },
    {
        id = "amber staff",
        chance = 3000,
    },
    {
        id = 6087,
        chance = 3000,
    },
    {
        id = 6088,
        chance = 3000,
    },
    {
        id = 6089,
        chance = 3000,
    },
    {
        id = 6090,
        chance = 3000,
    },
    {
        id = 3955,
        chance = 1000,
    },
    {
        id = "cultish mask",
        chance = 10250,
    },
    {
        id = 1962,
        chance = 910,
    },
    {
        id = "cultish symbol",
        chance = 890,
    },
    {
        id = "protection amulet",
        chance = 790,
    },
    {
        id = "small sapphire",
        chance = 550,
    },
    {
        id = "energy ring",
        chance = 450,
    },
    {
        id = "pirate voodoo doll",
        chance = 430,
    },
    {
        id = "platinum amulet",
        chance = 200,
    },
    {
        id = "mysterious voodoo skull",
        chance = 130,
    },
    {
        id = "broken key ring",
        chance = 100,
    },
    {
        id = "jewelled backpack",
        chance = 100,
    },
    {
        id = "blue robe",
        chance = 40,
    },
}

mtype:register(monster)
