local mtype = Game.createMonsterType("Fernfang")
local monster = {}

monster.name = "Fernfang"
monster.description = "Fernfang"
monster.experience = 600
monster.race = "blood"
monster.maxHealth = 400
monster.health = 400
monster.speed = 240
monster.manaCost = 0
monster.corpse = 20566
monster.outfit = { lookType = 206 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -50,
    },
    {
        name = "manadrain",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -25,
        maxDamage = -45,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "holy",
        interval = 2000,
        chance = 80,
        range = 7,
        minDamage = -65,
        maxDamage = -180,
        shootEffect = CONST_ANI_HOLY,
        effect = CONST_ME_HOLYAREA,
    },
}
monster.defenses = {
    defense = 10,
    armor = 15,
    {
        name = "healing",
        interval = 1000,
        chance = 15,
        minDamage = 90,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 7,
        speed = 310,
        duration = 10000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "effect",
        interval = 1000,
        chance = 5,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "outfit",
        interval = 1000,
        chance = 5,
        duration = 14000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 70},
    {type = COMBAT_EARTHDAMAGE, percent = 40},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "lifedrain", combat = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "War Wolf", interval = 1000, chance = 13, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You desecrated this place!", yell = false},
    {text = "I will cleanse this isle!", yell = false},
    {text = "Grrrrrrr", yell = false},
    {text = "Yoooohhuuuu!", yell = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 94,
    },
    {
        id = "book of prayers",
        chance = 100000,
    },
    {
        id = "platinum coin",
        chance = 97000,
        maxCount = 3,
    },
    {
        id = "star herb",
        chance = 92000,
    },
    {
        id = "rope belt",
        chance = 56000,
    },
    {
        id = "yellow gem",
        chance = 43000,
    },
    {
        id = "safety pin",
        chance = 41000,
    },
    {
        id = "power ring",
        chance = 41000,
    },
    {
        id = "blank rune",
        chance = 17000,
    },
    {
        id = "brown flask",
        chance = 12000,
    },
    {
        id = "bread",
        chance = 12000,
    },
    {
        id = 2044,
        chance = 12000,
    },
    {
        id = 2401,
        chance = 12000,
    },
    {
        id = "green tunic",
        chance = 12000,
    },
    {
        id = "strong mana potion",
        chance = 10200,
    },
    {
        id = "wooden whistle",
        chance = 7600,
    },
    {
        id = "sling herb",
        chance = 5100,
    },
    {
        id = "wolf tooth chain",
        chance = 5100,
    },
    {
        id = "life crystal",
        chance = 2500,
    },
}

mtype:register(monster)
