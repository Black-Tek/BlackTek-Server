local mtype = Game.createMonsterType("Primitive")
local monster = {}

monster.name = "Primitive"
monster.description = "Primitive"
monster.experience = 45
monster.race = "blood"
monster.maxHealth = 200
monster.health = 200
monster.speed = 300
monster.manaCost = 0
monster.corpse = 6080
monster.outfit = {
    lookType = 143,
    lookHead = 1,
    lookBody = 1,
    lookLegs = 1,
    lookFeet = 1,
}
monster.runHealth = 5
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -32,
    },
    {
        name = "drunk",
        interval = 1000,
        chance = 20,
        range = 7,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 34,
        range = 7,
        minDamage = -20,
        maxDamage = -20,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "outfit",
        interval = 1000,
        chance = 2,
        radius = 4,
        target = false,
        duration = 10000,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 25,
    armor = 20,
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "We don't need a future!", yell = false},
    {text = "I'll rook you all!", yell = false},
    {text = "They thought they'd beaten us!", yell = false},
    {text = "You are history!", yell = false},
    {text = "There can only be one world!", yell = false},
    {text = "Valor who?", yell = false},
    {text = "Die noob!", yell = false},
    {text = "There are no dragons!", yell = false},
    {text = "I'll quit forever! Again...", yell = false},
    {text = "You all are noobs!", yell = false},
    {text = "Beware of the cyclops!", yell = false},
    {text = "Just had a disconnect.", yell = false},
    {text = "Magic is only good for girls!", yell = false},
    {text = "We'll be back!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 12500,
        maxCount = 10,
    },
    {
        id = "sabre",
        chance = 10250,
    },
    {
        id = "axe",
        chance = 12250,
    },
    {
        id = "studded helmet",
        chance = 9500,
    },
    {
        id = 2484,
        chance = 7000,
    },
    {
        id = 2526,
        chance = 1200,
    },
    {
        id = 6570,
        chance = 500,
    },
    {
        id = 6571,
        chance = 500,
    },
}

mtype:register(monster)
