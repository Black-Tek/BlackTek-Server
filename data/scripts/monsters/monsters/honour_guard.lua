local mtype = Game.createMonsterType("Honour Guard")
local monster = {}

monster.name = "Honour Guard"
monster.description = "an honour guard"
monster.experience = 55
monster.race = "undead"
monster.maxHealth = 85
monster.health = 85
monster.speed = 154
monster.manaCost = 0
monster.corpse = 2843
monster.outfit = { lookType = 298 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -25,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -5,
        maxDamage = -13,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 4,
    armor = 4,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Horestis curse upon you!", yell = false},
    {text = "Vengeance!", yell = false},
    {text = "Thy punishment is at hand!", yell = false},
    {text = "For the pharao!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 15,
    },
    {
        id = "scarab coin",
        chance = 2600,
    },
    {
        id = "mace",
        chance = 3760,
    },
    {
        id = 2419,
        chance = 1640,
    },
    {
        id = "brown mushroom",
        chance = 6120,
        maxCount = 2,
    },
    {
        id = "pelvis bone",
        chance = 8700,
    },
}

mtype:register(monster)
