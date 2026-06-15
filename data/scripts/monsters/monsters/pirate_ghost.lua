local mtype = Game.createMonsterType("Pirate Ghost")
local monster = {}

monster.name = "Pirate Ghost"
monster.description = "a pirate ghost"
monster.experience = 250
monster.race = "undead"
monster.maxHealth = 275
monster.health = 275
monster.speed = 170
monster.manaCost = 0
monster.corpse = 5566
monster.outfit = { lookType = 196 }
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
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
        -- NOTE: melee condition (poison=40); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 40000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        minDamage = -40,
        maxDamage = -80,
        radius = 1,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -50,
        maxDamage = -65,
        radius = 3,
        target = true,
        effect = CONST_ME_SOUND_RED,
    },
}
monster.defenses = {
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 40,
        maxDamage = 70,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
    {type = "physical", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Yooh Ho Hooh Ho!", yell = false},
    {text = "Hell is waiting for You!", yell = false},
    {text = "It's alive!", yell = false},
    {text = "The curse! Aww the curse!", yell = false},
    {text = "You will not get my treasure!", yell = false},
}
monster.loot = {
    {
        id = 1951,
        chance = 910,
    },
    {
        id = "gold coin",
        chance = 48000,
        maxCount = 67,
    },
    {
        id = "stealth ring",
        chance = 650,
    },
    {
        id = "spike sword",
        chance = 130,
    },
    {
        id = "red robe",
        chance = 130,
    },
    {
        id = "tattered piece of robe",
        chance = 4300,
    },
}

mtype:register(monster)
