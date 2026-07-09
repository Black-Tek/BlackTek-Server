local mtype = Game.createMonsterType("Spit Nettle")
local monster = {}

monster.name = "Spit Nettle"
monster.description = "a spit nettle"
monster.experience = 20
monster.race = "venom"
monster.maxHealth = 150
monster.health = 150
monster.speed = 0
monster.manaCost = 0
monster.corpse = 6062
monster.outfit = { lookType = 221 }
monster.changeTarget = {
    interval = 4000,
    chance = 20,
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
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "earth",
        interval = 1000,
        chance = 20,
        range = 7,
        minDamage = -15,
        maxDamage = -40,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -40,
        maxDamage = -100,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 12,
    armor = 12,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 15,
        maxDamage = 37,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 2804,
        chance = 10954,
    },
    {
        id = 2148,
        chance = 10703,
        maxCount = 5,
    },
    {
        id = 12432,
        chance = 9840,
    },
    {
        id = 2802,
        chance = 5556,
        maxCount = 2,
    },
    {
        id = 2747,
        chance = 989,
    },
    {
        id = 11231,
        chance = 910,
    },
    {
        id = 7732,
        chance = 361,
    },
}

mtype:register(monster)
