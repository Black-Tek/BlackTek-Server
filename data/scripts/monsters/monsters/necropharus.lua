local mtype = Game.createMonsterType("Necropharus")
local monster = {}

monster.name = "Necropharus"
monster.description = "Necropharus"
monster.experience = 1050
monster.race = "blood"
monster.maxHealth = 750
monster.health = 750
monster.speed = 240
monster.manaCost = 0
monster.corpse = 20574
monster.outfit = { lookType = 209 }
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
        maxDamage = -102,
        -- NOTE: melee condition (poison=95); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 95000,
        },
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 20,
        range = 1,
        minDamage = -80,
        maxDamage = -120,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 17,
        range = 7,
        minDamage = -50,
        maxDamage = -140,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 60,
        maxDamage = 90,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "ghoul", interval = 1000, chance = 20, max = 2},
    {name = "ghost", interval = 1000, chance = 17, max = 1},
    {name = "mummy", interval = 1000, chance = 15, max = 1},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You will rise as my servant!", yell = false},
    {text = "Praise to my master Urgith!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "book of necromantic rituals",
        chance = 100000,
    },
    {
        id = "necromantic robe",
        chance = 100000,
    },
    {
        id = "soul stone",
        chance = 100000,
    },
    {
        id = "clerical mace",
        chance = 55000,
    },
    {
        id = "skull staff",
        chance = 44000,
    },
    {
        id = "bone club",
        chance = 36000,
    },
    {
        id = "green mushroom",
        chance = 25000,
    },
    {
        id = 2229,
        chance = 13000,
    },
    {
        id = "mystic turban",
        chance = 11000,
    },
    {
        id = "moonlight rod",
        chance = 8300,
    },
    {
        id = 2231,
        chance = 5500,
    },
    {
        id = "bone shield",
        chance = 5500,
    },
    {
        id = "boots of haste",
        chance = 2700,
    },
    {
        id = "strong mana potion",
        chance = 2700,
    },
}

mtype:register(monster)
