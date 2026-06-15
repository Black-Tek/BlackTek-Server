local mtype = Game.createMonsterType("Deepling Worker")
local monster = {}

monster.name = "Deepling Worker"
monster.description = "a deepling worker"
monster.experience = 130
monster.race = "blood"
monster.maxHealth = 190
monster.health = 190
monster.speed = 210
monster.manaCost = 0
monster.corpse = 15497
monster.outfit = { lookType = 470 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
    {type = COMBAT_EARTHDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Qjell afar gou jey!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 70000,
        maxCount = 25,
    },
    {
        id = "small emerald",
        chance = 110,
        maxCount = 3,
    },
    {
        id = 2667,
        chance = 12020,
        maxCount = 3,
    },
    {
        id = "fish fin",
        chance = 350,
    },
    {
        id = "heavy trident",
        chance = 510,
    },
    {
        id = "eye of a deepling",
        chance = 283,
    },
    {
        id = "deepling scales",
        chance = 6950,
    },
}

mtype:register(monster)
