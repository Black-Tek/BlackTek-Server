local mtype = Game.createMonsterType("Elder Mummy")
local monster = {}

monster.name = "Elder Mummy"
monster.description = "an elder mummy"
monster.experience = 560
monster.race = "undead"
monster.maxHealth = 850
monster.health = 850
monster.speed = 180
monster.manaCost = 0
monster.corpse = 6004
monster.outfit = {
    lookType = 65,
    lookHead = 20,
    lookBody = 30,
    lookLegs = 40,
    lookFeet = 50,
}
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
        -- NOTE: melee condition (poison=3); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 3000,
        },
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 1,
        minDamage = 0,
        maxDamage = -130,
        target = true,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -300,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 2124,
        chance = 1650,
    },
    {
        id = "silver brooch",
        chance = 4000,
    },
    {
        id = "black pearl",
        chance = 1340,
    },
    {
        id = "gold coin",
        chance = 43000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 43000,
        maxCount = 60,
    },
    {
        id = "scarab coin",
        chance = 10000,
        maxCount = 3,
    },
    {
        id = "strange talisman",
        chance = 4500,
    },
    {
        id = 2162,
        chance = 6000,
    },
    {
        id = "poison dagger",
        chance = 380,
    },
    {
        id = "worm",
        chance = 20000,
        maxCount = 3,
    },
    {
        id = "gauze bandage",
        chance = 10000,
    },
    {
        id = "flask of embalming fluid",
        chance = 12600,
    },
    {
        id = 13472,
        chance = 2400,
    },
}

mtype:register(monster)
