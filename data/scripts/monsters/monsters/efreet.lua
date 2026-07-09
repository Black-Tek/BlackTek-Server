local mtype = Game.createMonsterType("Efreet")
local monster = {}

monster.name = "Efreet"
monster.description = "an efreet"
monster.experience = 410
monster.race = "blood"
monster.maxHealth = 550
monster.health = 550
monster.speed = 234
monster.manaCost = 0
monster.corpse = 6032
monster.outfit = { lookType = 103 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -40,
        maxDamage = -110,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        minDamage = -30,
        maxDamage = -90,
        radius = 3,
        target = false,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -65,
        maxDamage = -120,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -650,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        duration = 6000,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        range = 7,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "djinn electrify",
        interval = 2000,
        chance = 15,
        range = 5,
    },
}
monster.defenses = {
    defense = 24,
    armor = 24,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 50,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 90},
    {type = COMBAT_ENERGYDAMAGE, percent = 60},
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -8},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "green djinn", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I grant you a deathwish!", yell = false},
    {text = "Good wishes are for fairytales", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 47000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 47000,
        maxCount = 30,
    },
    {
        id = "royal spear",
        chance = 15570,
        maxCount = 3,
    },
    {
        id = "strong mana potion",
        chance = 10500,
    },
    {
        id = "pear",
        chance = 9390,
        maxCount = 5,
    },
    {
        id = "jewelled belt",
        chance = 8540,
    },
    {
        id = "small emerald",
        chance = 7200,
    },
    {
        id = "heavy machete",
        chance = 5000,
    },
    {
        id = "green piece of cloth",
        chance = 3000,
    },
    {
        id = 1860,
        chance = 2200,
    },
    {
        id = "noble turban",
        chance = 1130,
    },
    {
        id = "magma monocle",
        chance = 420,
    },
    {
        id = "wand of inferno",
        chance = 390,
    },
    {
        id = "mystic turban",
        chance = 180,
    },
    {
        id = "small oil lamp",
        chance = 180,
    },
    {
        id = "green gem",
        chance = 150,
    },
}

mtype:register(monster)
