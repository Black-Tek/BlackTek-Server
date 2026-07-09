local mtype = Game.createMonsterType("Shadow Pupil")
local monster = {}

monster.name = "Shadow Pupil"
monster.description = "a shadow pupil"
monster.experience = 410
monster.race = "blood"
monster.maxHealth = 450
monster.health = 450
monster.speed = 210
monster.manaCost = 0
monster.corpse = 21254
monster.outfit = { lookType = 551 }
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -70,
        -- NOTE: melee condition (poison=90); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 90000,
        },
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -60,
        maxDamage = -80,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -30,
        maxDamage = -60,
        radius = 3,
        target = false,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Skeleton", interval = 2000, chance = 10, max = 0},
    {name = "Ghost", interval = 2000, chance = 5, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "When I'm finished with you, you'll be a shadow of your former self.", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 64,
    },
    {
        id = "boots of haste",
        chance = 60,
    },
    {
        id = "clerical mace",
        chance = 290,
    },
    {
        id = "mystic turban",
        chance = 580,
    },
    {
        id = "brown mushroom",
        chance = 5170,
    },
    {
        id = "strong mana potion",
        chance = 470,
    },
    {
        id = "spellbook of enlightenment",
        chance = 290,
    },
    {
        id = "book of necromantic rituals",
        chance = 6920,
    },
    {
        id = "horoscope",
        chance = 15000,
    },
    {
        id = "incantation notes",
        chance = 9130,
    },
    {
        id = "pieces of magic chalk",
        chance = 4300,
    },
}

mtype:register(monster)
