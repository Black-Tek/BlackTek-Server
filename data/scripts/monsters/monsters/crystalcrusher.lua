local mtype = Game.createMonsterType("Crystalcrusher")
local monster = {}

monster.name = "Crystalcrusher"
monster.description = "a crystalcrusher"
monster.experience = 500
monster.race = "venom"
monster.maxHealth = 570
monster.health = 570
monster.speed = 230
monster.manaCost = 0
monster.corpse = 18487
monster.outfit = { lookType = 511 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
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
        maxDamage = -167,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -110,
        maxDamage = -260,
        radius = 3,
        target = true,
        effect = CONST_ME_GREEN_RINGS,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 5,
        maxDamage = 15,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 129,
        maxDamage = 175,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 15},
    {type = COMBAT_HOLYDAMAGE, percent = -1},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -3},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Creak!", yell = false},
    {text = "Crunch!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 85000,
        maxCount = 90,
    },
    {
        id = "crystalline arrow",
        chance = 5000,
        maxCount = 3,
    },
    {
        id = "green crystal splinter",
        chance = 4920,
    },
    {
        id = "brown crystal splinter",
        chance = 5250,
    },
    {
        id = "blue crystal splinter",
        chance = 5000,
    },
    {
        id = "crystalline spikes",
        chance = 3690,
    },
}

mtype:register(monster)
