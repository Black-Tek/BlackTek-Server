local mtype = Game.createMonsterType("Mutated Human")
local monster = {}

monster.name = "Mutated Human"
monster.description = "a mutated human"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 240
monster.health = 240
monster.speed = 154
monster.manaCost = 0
monster.corpse = 9107
monster.outfit = { lookType = 323 }
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
    illusionable = true,
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
        maxDamage = -90,
        -- NOTE: melee condition (poison=60); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 60000,
        },
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -50,
        maxDamage = -60,
        length = 3,
        spread = 1,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 20,
        minDamage = -190,
        maxDamage = -280,
        length = 1,
        target = true,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        speed = -600,
        duration = 30000,
        effect = CONST_ME_EXPLOSIONHIT,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 220,
        duration = 5000,
        effect = CONST_ME_GREEN_RINGS,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
    {type = "drunk", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Take that creature off my back!! I can fell it!", yell = false},
    {text = "HEEEEEEEELP!", yell = false},
    {text = "You will be the next infected one... GRAAAAAAAAARRR!", yell = false},
    {text = "Science... is a curse.", yell = false},
    {text = "Run as fast as you can.", yell = false},
    {text = "Oh by the gods! What is this... aaaaaargh!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 88222,
        maxCount = 130,
    },
    {
        id = 11225,
        chance = 19936,
    },
    {
        id = 2226,
        chance = 10064,
    },
    {
        id = 3607,
        chance = 8076,
    },
    {
        id = 2483,
        chance = 7833,
    },
    {
        id = 3976,
        chance = 7081,
        maxCount = 2,
    },
    {
        id = 2376,
        chance = 5069,
    },
    {
        id = 2161,
        chance = 4946,
    },
    {
        id = 9808,
        chance = 2007,
    },
    {
        id = 7910,
        chance = 534,
    },
    {
        id = 2801,
        chance = 421,
    },
    {
        id = 2170,
        chance = 102,
    },
}

mtype:register(monster)
