local mtype = Game.createMonsterType("Lizard Legionnaire")
local monster = {}

monster.name = "Lizard Legionnaire"
monster.description = "a lizard legionnaire"
monster.experience = 1100
monster.race = "blood"
monster.maxHealth = 1400
monster.health = 1400
monster.speed = 230
monster.manaCost = 0
monster.corpse = 11276
monster.outfit = { lookType = 338 }
monster.runHealth = 10
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
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -180,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 40,
        range = 7,
        minDamage = 0,
        maxDamage = -200,
        target = true,
        shootEffect = CONST_ANI_SPEAR,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 45},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Tssss!", yell = false},
}
monster.loot = {
    {
        id = "small diamond",
        chance = 1001,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 44000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 54000,
        maxCount = 65,
    },
    {
        id = "lizard leather",
        chance = 970,
    },
    {
        id = "lizard scale",
        chance = 980,
        maxCount = 3,
    },
    {
        id = "strong health potion",
        chance = 3880,
    },
    {
        id = "red lantern",
        chance = 530,
    },
    {
        id = "bunch of ripe rice",
        chance = 1950,
    },
    {
        id = "Zaoan armor",
        chance = 70,
    },
    {
        id = "Zaoan shoes",
        chance = 460,
    },
    {
        id = "drakinata",
        chance = 710,
    },
    {
        id = "Zaoan halberd",
        chance = 960,
    },
    {
        id = "legionnaire flags",
        chance = 1940,
    },
    {
        id = "broken halberd",
        chance = 14940,
    },
    {
        id = "lizard trophy",
        chance = 20,
    },
}

mtype:register(monster)
