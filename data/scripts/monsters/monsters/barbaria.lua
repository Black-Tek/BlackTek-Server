local mtype = Game.createMonsterType("Barbaria")
local monster = {}

monster.name = "Barbaria"
monster.description = "Barbaria"
monster.experience = 355
monster.race = "blood"
monster.maxHealth = 345
monster.health = 345
monster.speed = 280
monster.manaCost = 0
monster.corpse = 20339
monster.outfit = {
    lookType = 264,
    lookHead = 78,
    lookBody = 97,
    lookLegs = 95,
    lookFeet = 120,
}
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -70,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 34,
        range = 7,
        minDamage = -30,
        maxDamage = -80,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SNOWBALL,
    },
    {
        name = "energy",
        interval = 3000,
        chance = 20,
        range = 7,
        minDamage = -35,
        maxDamage = -70,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 50,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "War Wolf", interval = 2000, chance = 40, max = 0},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "To me, creatures of the wild!", yell = false},
    {text = "My instincts tell me about your cowardice.", yell = false},
}
monster.loot = {
    {
        id = 1958,
        chance = 15000,
    },
    {
        id = 2050,
        chance = 25000,
    },
    {
        id = "gold coin",
        chance = 48000,
        maxCount = 35,
    },
    {
        id = "chain armor",
        chance = 11000,
    },
    {
        id = "hunting spear",
        chance = 12500,
    },
    {
        id = "fur bag",
        chance = 1000,
    },
}

mtype:register(monster)
