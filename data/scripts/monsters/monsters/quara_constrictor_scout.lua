local mtype = Game.createMonsterType("Quara Constrictor Scout")
local monster = {}

monster.name = "Quara Constrictor Scout"
monster.description = "a quara constrictor scout"
monster.experience = 200
monster.race = "blood"
monster.maxHealth = 450
monster.health = 450
monster.speed = 150
monster.manaCost = 670
monster.corpse = 6065
monster.outfit = { lookType = 46 }
monster.runHealth = 20
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
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -135,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        radius = 3,
        target = false,
        effect = CONST_ME_HITAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gaaahhh!", yell = false},
    {text = "Gluh! Gluh!", yell = false},
    {text = "Tssss!", yell = false},
    {text = "Boohaa!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99953,
        maxCount = 50,
    },
    {
        id = 12443,
        chance = 15123,
    },
    {
        id = 2670,
        chance = 10015,
    },
    {
        id = 2397,
        chance = 8090,
    },
    {
        id = 2465,
        chance = 5076,
    },
    {
        id = 2150,
        chance = 4015,
    },
    {
        id = 5895,
        chance = 507,
    },
}

mtype:register(monster)
