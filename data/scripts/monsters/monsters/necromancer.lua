local mtype = Game.createMonsterType("Necromancer")
local monster = {}

monster.name = "Necromancer"
monster.description = "a necromancer"
monster.experience = 580
monster.race = "blood"
monster.maxHealth = 580
monster.health = 580
monster.speed = 188
monster.manaCost = 0
monster.corpse = 20455
monster.outfit = { lookType = 9 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
        -- NOTE: melee condition (poison=160); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 160000,
        },
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 1,
        minDamage = -60,
        maxDamage = -120,
        target = true,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -65,
        maxDamage = -120,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 50,
    armor = 50,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 50,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Ghoul", interval = 2000, chance = 15, max = 0},
    {name = "Ghost", interval = 2000, chance = 5, max = 0},
    {name = "Mummy", interval = 2000, chance = 5, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Your corpse will be mine!", yell = false},
    {text = "Taste the sweetness of death!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 30050,
        maxCount = 90,
    },
    {
        id = "poison arrow",
        chance = 15000,
        maxCount = 5,
    },
    {
        id = "book of necromantic rituals",
        chance = 10130,
    },
    {
        id = "green mushroom",
        chance = 1470,
    },
    {
        id = "necromantic robe",
        chance = 1001,
    },
    {
        id = "mystic turban",
        chance = 500,
    },
    {
        id = "clerical mace",
        chance = 390,
    },
    {
        id = "strong mana potion",
        chance = 300,
    },
    {
        id = "boots of haste",
        chance = 210,
    },
    {
        id = "spellbook of warding",
        chance = 130,
    },
    {
        id = "skull staff",
        chance = 100,
    },
    {
        id = "noble axe",
        chance = 10,
    },
}

mtype:register(monster)
