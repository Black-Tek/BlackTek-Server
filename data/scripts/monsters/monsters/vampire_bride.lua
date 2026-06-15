local mtype = Game.createMonsterType("Vampire Bride")
local monster = {}

monster.name = "Vampire Bride"
monster.description = "a vampire bride"
monster.experience = 1050
monster.race = "blood"
monster.maxHealth = 1200
monster.health = 1200
monster.speed = 230
monster.manaCost = 0
monster.corpse = 9658
monster.outfit = { lookType = 312 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -190,
    },
    {
        name = "lifedrain",
        interval = 3000,
        chance = 15,
        range = 1,
        minDamage = -60,
        maxDamage = -130,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -60,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "death",
        interval = 4000,
        chance = 5,
        range = 7,
        minDamage = -60,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_HEARTS,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 40,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_DROWNDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Kneel before your Mistress!", yell = false},
    {text = "Dead is the new alive.", yell = false},
    {text = "Come, let me kiss you, darling. Oh wait, I meant kill.", yell = false},
    {text = "Enjoy the pain - I know you love it.", yell = false},
    {text = "Are you suffering nicely enough?", yell = false},
    {text = "You won't regret you came to me, sweetheart.", yell = false},
}
monster.loot = {
    {
        id = "emerald bangle",
        chance = 1100,
    },
    {
        id = "small diamond",
        chance = 1020,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 45000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 45000,
        maxCount = 49,
    },
    {
        id = "platinum coin",
        chance = 9910,
    },
    {
        id = "moonlight rod",
        chance = 5500,
    },
    {
        id = "boots of haste",
        chance = 220,
    },
    {
        id = "enigmatic voodoo skull",
        chance = 170,
    },
    {
        id = "strong health potion",
        chance = 5000,
    },
    {
        id = "strong mana potion",
        chance = 10210,
    },
    {
        id = 7733,
        chance = 200,
    },
    {
        id = "hibiscus dress",
        chance = 1030,
    },
    {
        id = "blood goblet",
        chance = 60,
    },
    {
        id = 9809,
        chance = 1010,
    },
    {
        id = 9837,
        chance = 970,
    },
    {
        id = "vampire teeth",
        chance = 10000,
    },
    {
        id = "blood preservation",
        chance = 4950,
    },
    {
        id = "leather whip",
        chance = 20,
    },
}

mtype:register(monster)
