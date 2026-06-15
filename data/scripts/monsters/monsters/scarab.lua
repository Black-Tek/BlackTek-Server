local mtype = Game.createMonsterType("Scarab")
local monster = {}

monster.name = "Scarab"
monster.description = "a scarab"
monster.experience = 120
monster.race = "venom"
monster.maxHealth = 320
monster.health = 320
monster.speed = 160
monster.manaCost = 395
monster.corpse = 6024
monster.outfit = { lookType = 83 }
monster.runHealth = 80
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
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
        maxDamage = -75,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = 0,
        maxDamage = -35,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "poisonfield",
        interval = 2000,
        chance = 10,
        radius = 1,
        target = false,
    },
}
monster.defenses = {
    defense = 21,
    armor = 21,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 200,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -18},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 87000,
        maxCount = 52,
    },
    {
        id = "meat",
        chance = 40000,
        maxCount = 2,
    },
    {
        id = "piece of scarab shell",
        chance = 5000,
    },
    {
        id = "scarab coin",
        chance = 1100,
    },
    {
        id = "small amethyst",
        chance = 540,
    },
    {
        id = "small emerald",
        chance = 400,
    },
    {
        id = "daramian mace",
        chance = 300,
    },
}

mtype:register(monster)
