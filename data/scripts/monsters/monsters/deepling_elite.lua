local mtype = Game.createMonsterType("Deepling Elite")
local monster = {}

monster.name = "Deepling Elite"
monster.description = "a deepling elite"
monster.experience = 3000
monster.race = "blood"
monster.maxHealth = 3200
monster.health = 3200
monster.speed = 210
monster.manaCost = 0
monster.corpse = 15176
monster.outfit = { lookType = 441 }
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
        maxDamage = -400,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -225,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 150,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 86,
    },
    {
        id = "small emerald",
        chance = 6290,
        maxCount = 2,
    },
    {
        id = "life ring",
        chance = 5360,
    },
    {
        id = "fish fin",
        chance = 2000,
    },
    {
        id = "great mana potion",
        chance = 24000,
    },
    {
        id = "great health potion",
        chance = 25000,
    },
    {
        id = "heavy trident",
        chance = 3380,
    },
    {
        id = "eye of a deepling",
        chance = 25000,
    },
    {
        id = "deepling warts",
        chance = 25000,
    },
    {
        id = "deeptags",
        chance = 21700,
    },
    {
        id = "warrior's axe",
        chance = 640,
    },
    {
        id = "deepling ridge",
        chance = 19000,
    },
    {
        id = "warrior's shield",
        chance = 1234,
    },
    {
        id = "deepling filet",
        chance = 25000,
    },
    {
        id = "vortex bolt",
        chance = 24000,
        maxCount = 5,
    },
}

mtype:register(monster)
