local mtype = Game.createMonsterType("The Weakened Count")
local monster = {}

monster.name = "The Weakened Count"
monster.description = "The Weakened Count"
monster.experience = 450
monster.race = "undead"
monster.maxHealth = 740
monster.health = 740
monster.speed = 238
monster.manaCost = 0
monster.corpse = 6006
monster.outfit = { lookType = 68 }
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -190,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        range = 1,
        minDamage = 0,
        maxDamage = -95,
        target = true,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 1,
        target = true,
        speed = -400,
        duration = 60000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 15,
        maxDamage = 25,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "1... 2... 2... Uh, can't concentrate.", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 92,
    },
    {
        id = "the ring of the count",
        chance = 100000,
    },
    {
        id = "vampire teeth",
        chance = 7600,
    },
    {
        id = "blood preservation",
        chance = 5100,
    },
    {
        id = 2747,
        chance = 1910,
    },
    {
        id = "black pearl",
        chance = 1800,
    },
    {
        id = "strong health potion",
        chance = 1500,
    },
    {
        id = "katana",
        chance = 1560,
    },
    {
        id = "spike sword",
        chance = 1000,
    },
    {
        id = 2229,
        chance = 1000,
    },
    {
        id = "ice rapier",
        chance = 470,
    },
    {
        id = "strange helmet",
        chance = 420,
    },
    {
        id = "bronze amulet",
        chance = 230,
    },
    {
        id = "emerald bangle",
        chance = 210,
    },
    {
        id = "vampire shield",
        chance = 200,
    },
}

mtype:register(monster)
