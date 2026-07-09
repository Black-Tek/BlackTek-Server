local mtype = Game.createMonsterType("Enlightened of the Cult")
local monster = {}

monster.name = "Enlightened of the Cult"
monster.description = "an enlightened of the cult"
monster.experience = 500
monster.race = "blood"
monster.maxHealth = 700
monster.health = 700
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
        -- NOTE: melee condition (poison=4); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 4000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 25,
        range = 1,
        minDamage = -70,
        maxDamage = -185,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_HOLY,
        effect = CONST_ME_HOLYAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        duration = 5000,
        shootEffect = CONST_ANI_HOLY,
        effect = CONST_ME_HOLYDAMAGE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        speed = -360,
        duration = 6000,
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
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Crypt Shambler", interval = 2000, chance = 10, max = 0},
    {name = "Ghost", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Praise to my master Urgith!", yell = false},
    {text = "You will rise as my servant!", yell = false},
    {text = "Praise to my masters! Long live the triangle!", yell = false},
    {text = "You will die in the name of the triangle!", yell = false},
}
monster.loot = {
    {
        id = 1962,
        chance = 910,
    },
    {
        id = "piggy bank",
        chance = 130,
    },
    {
        id = "small sapphire",
        chance = 550,
    },
    {
        id = "gold coin",
        chance = 64550,
        maxCount = 70,
    },
    {
        id = "energy ring",
        chance = 450,
    },
    {
        id = "platinum amulet",
        chance = 200,
    },
    {
        id = "wand of inferno",
        chance = 180,
    },
    {
        id = "protection amulet",
        chance = 790,
    },
    {
        id = "skull staff",
        chance = 350,
    },
    {
        id = "blue robe",
        chance = 40,
    },
    {
        id = "enigmatic voodoo skull",
        chance = 130,
    },
    {
        id = "jewelled backpack",
        chance = 100,
    },
    {
        id = "pirate voodoo doll",
        chance = 430,
    },
    {
        id = 6090,
        chance = 490,
    },
    {
        id = "amber staff",
        chance = 100,
    },
    {
        id = "strong mana potion",
        chance = 740,
    },
    {
        id = "cultish mask",
        chance = 10250,
    },
    {
        id = "cultish symbol",
        chance = 890,
    },
    {
        id = "broken key ring",
        chance = 100,
    },
}

mtype:register(monster)
