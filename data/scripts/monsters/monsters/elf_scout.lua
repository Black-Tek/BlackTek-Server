local mtype = Game.createMonsterType("Elf Scout")
local monster = {}

monster.name = "Elf Scout"
monster.description = "an elf scout"
monster.experience = 75
monster.race = "blood"
monster.maxHealth = 160
monster.health = 160
monster.speed = 220
monster.manaCost = 360
monster.corpse = 6012
monster.outfit = { lookType = 64 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -30,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -80,
        shootEffect = CONST_ANI_ARROW,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Tha'shi Ab'Dendriel!", yell = false},
    {text = "Feel the sting of my arrows!", yell = false},
    {text = "Thy blood will quench the soil's thirst!", yell = false},
    {text = "Evicor guide my arrow.", yell = false},
    {text = "Your existence will end here!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 75000,
        maxCount = 25,
    },
    {
        id = 2544,
        chance = 30710,
        maxCount = 12,
    },
    {
        id = "grapes",
        chance = 17750,
    },
    {
        id = "poison arrow",
        chance = 15400,
        maxCount = 4,
    },
    {
        id = "elven scouting glass",
        chance = 9750,
    },
    {
        id = "elvish talisman",
        chance = 5200,
    },
    {
        id = "bow",
        chance = 4000,
    },
    {
        id = "waterskin",
        chance = 1350,
    },
    {
        id = "sandals",
        chance = 1180,
    },
    {
        id = 5921,
        chance = 1130,
    },
    {
        id = "elvish bow",
        chance = 140,
    },
}

mtype:register(monster)
