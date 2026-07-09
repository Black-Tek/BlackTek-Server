local mtype = Game.createMonsterType("Firestarter")
local monster = {}

monster.name = "Firestarter"
monster.description = "a firestarter"
monster.experience = 80
monster.race = "blood"
monster.maxHealth = 180
monster.health = 180
monster.speed = 180
monster.manaCost = 0
monster.corpse = 20599
monster.outfit = {
    lookType = 159,
    lookHead = 94,
    lookBody = 77,
    lookLegs = 78,
    lookFeet = 79,
}
monster.changeTarget = {
    interval = 4000,
    chance = 0,
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
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -15,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = 0,
        maxDamage = -21,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_BURSTARROW,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -20},
    {type = COMBAT_ICEDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "FIRE!", yell = true},
    {text = "BURN!", yell = true},
    {text = "DEATH to the FALSE GOD!!", yell = true},
    {text = "You shall burn in the thornfires!!", yell = false},
    {text = "DOWN with the followers of the bog!!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 79000,
        maxCount = 35,
    },
    {
        id = "longsword",
        chance = 6000,
    },
    {
        id = "bow",
        chance = 4000,
    },
    {
        id = "grapes",
        chance = 20000,
    },
    {
        id = 5921,
        chance = 930,
    },
    {
        id = "elvish bow",
        chance = 100,
    },
    {
        id = "flaming arrow",
        chance = 30000,
        maxCount = 12,
    },
    {
        id = "elvish talisman",
        chance = 5000,
    },
    {
        id = 13757,
        chance = 15280,
    },
    {
        id = "flintstone",
        chance = 340,
    },
}

mtype:register(monster)
