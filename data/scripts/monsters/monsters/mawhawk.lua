local mtype = Game.createMonsterType("Mawhawk")
local monster = {}

monster.name = "Mawhawk"
monster.description = "Mawhawk"
monster.experience = 14000
monster.race = "blood"
monster.maxHealth = 45000
monster.health = 45000
monster.speed = 270
monster.manaCost = 0
monster.corpse = 22629
monster.outfit = { lookType = 595 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -450,
    },
    {
        name = "earth",
        interval = 1800,
        chance = 10,
        minDamage = -300,
        maxDamage = -685,
        length = 7,
        effect = CONST_ME_STONES,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 9,
        minDamage = -250,
        maxDamage = -590,
        radius = 6,
        effect = CONST_ME_BIGPLANTS,
    },
}
monster.defenses = {
    defense = 55,
    armor = 55,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Knorrrr!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 10000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 10000,
        maxCount = 25,
    },
    {
        id = "fire sword",
        chance = 10000,
    },
    {
        id = "iron ore",
        chance = 10000,
    },
    {
        id = "fish fin",
        chance = 10000,
    },
    {
        id = "red piece of cloth",
        chance = 10000,
    },
    {
        id = "hardened bone",
        chance = 10000,
    },
    {
        id = "assassin dagger",
        chance = 10000,
    },
    {
        id = "haunted blade",
        chance = 10000,
    },
    {
        id = "nightmare blade",
        chance = 10000,
    },
    {
        id = "violet crystal shard",
        chance = 10000,
        maxCount = 3,
    },
    {
        id = "green crystal shard",
        chance = 10000,
        maxCount = 3,
    },
    {
        id = "green crystal splinter",
        chance = 10000,
        maxCount = 5,
    },
    {
        id = "blue crystal splinter",
        chance = 10000,
        maxCount = 5,
    },
    {
        id = "cluster of solace",
        chance = 30000,
        maxCount = 2,
    },
    {
        id = "frazzle tongue",
        chance = 30000,
    },
    {
        id = "unrealized dream",
        chance = 30000,
        maxCount = 2,
    },
}

mtype:register(monster)
