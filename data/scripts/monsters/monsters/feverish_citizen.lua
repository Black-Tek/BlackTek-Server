local mtype = Game.createMonsterType("Feverish Citizen")
local monster = {}

monster.name = "Feverish Citizen"
monster.description = "a feverish citizen"
monster.experience = 30
monster.race = "blood"
monster.maxHealth = 125
monster.health = 125
monster.speed = 185
monster.manaCost = 0
monster.corpse = 20395
monster.outfit = {
    lookType = 425,
    lookAddons = 2,
}
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
    pushable = true,
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
        maxDamage = -18,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 15,
        length = 3,
        spread = 2,
        target = false,
        duration = 3000,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        radius = 3,
        target = false,
        duration = 5000,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 80},
    {type = COMBAT_FIREDAMAGE, percent = 20},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Aaaaargh!", yell = false},
    {text = "I am the king of the world!", yell = false},
    {text = "Die Ferumbras!", yell = false},
    {text = "Tigerblood is running through my veins!", yell = false},
    {text = "You! It's you again!", yell = false},
    {text = "Stand still you tasty morsel!", yell = false},
    {text = "<giggle>", yell = false},
    {text = "Burn heretic! Burn!", yell = false},
    {text = "Harrr!", yell = false},
    {text = "This is Venore!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 30300,
        maxCount = 35,
    },
    {
        id = 2230,
        chance = 4920,
    },
    {
        id = "worm",
        chance = 25730,
        maxCount = 3,
    },
    {
        id = "ominous piece of cloth",
        chance = 1660,
    },
    {
        id = "dubious piece of cloth",
        chance = 1720,
    },
    {
        id = "voluminous piece of cloth",
        chance = 2290,
    },
    {
        id = "obvious piece of cloth",
        chance = 2006,
    },
    {
        id = "ludicrous piece of cloth",
        chance = 2060,
    },
    {
        id = "luminous piece of cloth",
        chance = 2290,
    },
    {
        id = "plague mask",
        chance = 100,
    },
    {
        id = "plague bell",
        chance = 100,
    },
}

mtype:register(monster)
