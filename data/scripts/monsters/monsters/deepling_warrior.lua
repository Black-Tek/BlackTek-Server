local mtype = Game.createMonsterType("Deepling Warrior")
local monster = {}

monster.name = "Deepling Warrior"
monster.description = "a deepling warrior"
monster.experience = 1500
monster.race = "blood"
monster.maxHealth = 1600
monster.health = 1600
monster.speed = 210
monster.manaCost = 0
monster.corpse = 15175
monster.outfit = { lookType = 441 }
monster.runHealth = 30
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -290,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 50,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -20},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Jou wjil all djie!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 80,
    },
    {
        id = "small emerald",
        chance = 2854,
    },
    {
        id = "life ring",
        chance = 2941,
    },
    {
        id = "fish fin",
        chance = 961,
    },
    {
        id = "great mana potion",
        chance = 9090,
    },
    {
        id = "great health potion",
        chance = 11111,
    },
    {
        id = "heavy trident",
        chance = 1030,
    },
    {
        id = "eye of a deepling",
        chance = 574,
    },
    {
        id = "deepling warts",
        chance = 10000,
    },
    {
        id = "deeptags",
        chance = 14285,
    },
    {
        id = "warrior's axe",
        chance = 534,
    },
    {
        id = "deepling ridge",
        chance = 11111,
    },
    {
        id = "warrior's shield",
        chance = 684,
    },
    {
        id = "deepling filet",
        chance = 14285,
    },
    {
        id = "vortex bolt",
        chance = 3571,
        maxCount = 5,
    },
}

mtype:register(monster)
