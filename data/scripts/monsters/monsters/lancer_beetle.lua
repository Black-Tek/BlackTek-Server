local mtype = Game.createMonsterType("Lancer Beetle")
local monster = {}

monster.name = "Lancer Beetle"
monster.description = "a lancer beetle"
monster.experience = 275
monster.race = "venom"
monster.maxHealth = 400
monster.health = 400
monster.speed = 200
monster.manaCost = 0
monster.corpse = 11375
monster.outfit = { lookType = 348 }
monster.runHealth = 30
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
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -115,
    },
    {
        name = "poisonfield",
        interval = 2000,
        chance = 10,
        radius = 4,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        minDamage = 0,
        maxDamage = -90,
        length = 7,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -40,
        maxDamage = -80,
        target = false,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "lancer beetle curse",
        interval = 2000,
        chance = 5,
        range = 5,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 3000,
        effect = CONST_ME_GROUNDSHAKER,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 50},
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
        chance = 60000,
        maxCount = 61,
    },
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 79,
    },
    {
        id = "small amethyst",
        chance = 247,
    },
    {
        id = "poisonous slime",
        chance = 8333,
    },
    {
        id = "lump of dirt",
        chance = 4166,
    },
    {
        id = "lancer beetle shell",
        chance = 16666,
    },
    {
        id = "beetle necklace",
        chance = 1123,
    },
}

mtype:register(monster)
