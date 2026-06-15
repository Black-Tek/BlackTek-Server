local mtype = Game.createMonsterType("Starving Wolf")
local monster = {}

monster.name = "Starving Wolf"
monster.description = "a starving wolf"
monster.experience = 65
monster.race = "blood"
monster.maxHealth = 85
monster.health = 85
monster.speed = 188
monster.manaCost = 0
monster.corpse = 5968
monster.outfit = { lookType = 27 }
monster.runHealth = 8
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -25,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
    {
        name = "effect",
        interval = 2000,
        chance = 10,
        radius = 1,
        effect = CONST_ME_YELLOW_RINGS,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gaarrrrrr", yell = false},
    {text = "Gnoorrr", yell = false},
    {text = "Yoooohhuuuu!", yell = false},
}
monster.loot = {
    {
        id = 2220,
        chance = 64730,
    },
    {
        id = "meat",
        chance = 5300,
        maxCount = 2,
    },
    {
        id = "wolf paw",
        chance = 1430,
    },
}

mtype:register(monster)
