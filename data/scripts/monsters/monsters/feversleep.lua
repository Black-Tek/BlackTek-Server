local mtype = Game.createMonsterType("Feversleep")
local monster = {}

monster.name = "Feversleep"
monster.description = "a feversleep"
monster.experience = 4400
monster.race = "blood"
monster.maxHealth = 5900
monster.health = 5900
monster.speed = 270
monster.manaCost = 0
monster.corpse = 22497
monster.outfit = { lookType = 593 }
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -450,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 20,
        minDamage = -800,
        maxDamage = -1000,
        radius = 7,
        target = false,
        effect = CONST_ME_YELLOW_RINGS,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -70,
        maxDamage = -100,
        radius = 5,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "feversleep skill reducer",
        interval = 2000,
        chance = 10,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -250,
        maxDamage = -300,
        length = 6,
        target = true,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -150,
        maxDamage = -300,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 250,
        maxDamage = 425,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_HITAREA,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "small emerald",
        chance = 11000,
        maxCount = 2,
    },
    {
        id = "small amethyst",
        chance = 12000,
        maxCount = 3,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 9,
    },
    {
        id = "blue robe",
        chance = 1500,
    },
    {
        id = "great mana potion",
        chance = 40000,
        maxCount = 2,
    },
    {
        id = "ultimate health potion",
        chance = 18000,
    },
    {
        id = "small topaz",
        chance = 16000,
        maxCount = 2,
    },
    {
        id = "blue crystal shard",
        chance = 11000,
    },
    {
        id = "blue crystal splinter",
        chance = 13000,
    },
    {
        id = "cyan crystal fragment",
        chance = 18000,
    },
    {
        id = "trapped bad dream monster",
        chance = 17000,
    },
    {
        id = "bowl of terror sweat",
        chance = 14000,
    },
}

mtype:register(monster)
