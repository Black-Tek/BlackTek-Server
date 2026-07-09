local mtype = Game.createMonsterType("The Blightfather")
local monster = {}

monster.name = "The Blightfather"
monster.description = "The Blightfather"
monster.experience = 600
monster.race = "venom"
monster.maxHealth = 400
monster.health = 400
monster.speed = 290
monster.manaCost = 0
monster.corpse = 11375
monster.outfit = { lookType = 348 }
monster.runHealth = 80
monster.changeTarget = {
    interval = 5000,
    chance = 12,
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
        maxDamage = -200,
    },
    {
        name = "effect",
        interval = 2000,
        chance = 20,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "invisible",
        interval = 1000,
        chance = 10,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Crump!", yell = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 2000,
        maxCount = 61,
    },
    {
        id = "gold coin",
        chance = 2000,
        maxCount = 60,
    },
    {
        id = "small amethyst",
        chance = 800,
    },
    {
        id = "poisonous slime",
        chance = 12500,
    },
    {
        id = "lump of dirt",
        chance = 17500,
    },
    {
        id = "lancer beetle shell",
        chance = 7000,
    },
    {
        id = "beetle necklace",
        chance = 400,
    },
}

mtype:register(monster)
