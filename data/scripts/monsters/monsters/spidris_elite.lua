local mtype = Game.createMonsterType("Spidris Elite")
local monster = {}

monster.name = "Spidris Elite"
monster.description = "Spidris Elite"
monster.experience = 4000
monster.race = "venom"
monster.maxHealth = 5000
monster.health = 5000
monster.speed = 260
monster.manaCost = 0
monster.corpse = 15296
monster.outfit = { lookType = 457 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -349,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 23280,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 45000,
        maxCount = 6,
    },
    {
        id = "violet gem",
        chance = 1120,
    },
    {
        id = 6300,
        chance = 4480,
    },
    {
        id = "titan axe",
        chance = 1440,
    },
    {
        id = "great mana potion",
        chance = 20400,
        maxCount = 2,
    },
    {
        id = 7632,
        chance = 3040,
    },
    {
        id = "ultimate health potion",
        chance = 9250,
        maxCount = 2,
    },
    {
        id = "spidris mandible",
        chance = 27440,
    },
    {
        id = "compound eye",
        chance = 13210,
    },
    {
        id = "calopteryx cape",
        chance = 1280,
    },
    {
        id = "carapace shield",
        chance = 1170,
    },
    {
        id = "hive scythe",
        chance = 1390,
    },
}

mtype:register(monster)
