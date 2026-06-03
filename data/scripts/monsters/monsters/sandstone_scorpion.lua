local mtype = Game.createMonsterType("Sandstone Scorpion")
local monster = {}

monster.name = "Sandstone Scorpion"
monster.description = "a sandstone scorpion"
monster.experience = 680
monster.race = "undead"
monster.maxHealth = 900
monster.health = 900
monster.speed = 230
monster.manaCost = 0
monster.corpse = 13501
monster.outfit = { lookType = 398 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -193,
        -- NOTE: melee condition (poison=1000); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 1000000,
        },
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 60,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = -1},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "*rattle*", yell = false},
    {text = "*tak tak*", yell = false},
    {text = "*tak tak tak*", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 45000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 45000,
        maxCount = 28,
    },
    {
        id = "small emerald",
        chance = 9900,
        maxCount = 2,
    },
    {
        id = "platinum coin",
        chance = 11111,
        maxCount = 2,
    },
    {
        id = "daramian mace",
        chance = 6250,
    },
    {
        id = "steel helmet",
        chance = 5000,
    },
    {
        id = "black shield",
        chance = 1724,
    },
    {
        id = "fist on a stick",
        chance = 280,
    },
}

mtype:register(monster)
