local mtype = Game.createMonsterType("Massive Energy Elemental")
local monster = {}

monster.name = "Massive Energy Elemental"
monster.description = "a massive energy elemental"
monster.experience = 950
monster.race = "energy"
monster.maxHealth = 1100
monster.health = 1100
monster.speed = 210
monster.manaCost = 0
monster.corpse = 8966
monster.outfit = { lookType = 290 }
monster.changeTarget = {
    interval = 4000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -175,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -270,
        maxDamage = -615,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -175,
        maxDamage = -205,
        target = true,
        shootEffect = CONST_ANI_ENERGYBALL,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "massive energy elemental electrify",
        interval = 2000,
        chance = 20,
        effect = CONST_ME_BLOCKHIT,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 190,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 70},
    {type = COMBAT_HOLYDAMAGE, percent = 25},
    {type = COMBAT_DEATHDAMAGE, percent = 1},
    {type = COMBAT_EARTHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 91,
    },
    {
        id = "small amethyst",
        chance = 3270,
        maxCount = 3,
    },
    {
        id = "strong mana potion",
        chance = 17450,
    },
    {
        id = "great mana potion",
        chance = 5450,
    },
    {
        id = "flash arrow",
        chance = 6550,
        maxCount = 15,
    },
    {
        id = "energy spike sword",
        chance = 500,
    },
    {
        id = "lightning pendant",
        chance = 730,
    },
    {
        id = "lightning legs",
        chance = 150,
    },
    {
        id = "spellbook of warding",
        chance = 360,
    },
    {
        id = "wand of starstorm",
        chance = 730,
    },
    {
        id = 9809,
        chance = 730,
    },
    {
        id = "shockwave amulet",
        chance = 500,
    },
}

mtype:register(monster)
