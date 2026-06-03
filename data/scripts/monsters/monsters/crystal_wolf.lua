local mtype = Game.createMonsterType("Crystal Wolf")
local monster = {}

monster.name = "Crystal Wolf"
monster.description = "a crystal wolf"
monster.experience = 275
monster.race = "undead"
monster.maxHealth = 750
monster.health = 750
monster.speed = 200
monster.manaCost = 0
monster.corpse = 13584
monster.outfit = { lookType = 391 }
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -60,
        maxDamage = -130,
        length = 3,
        spread = 2,
        effect = CONST_ME_STONES,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 15,
        range = 6,
        minDamage = -80,
        maxDamage = -150,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_SMALLICE,
        effect = CONST_ME_GIANTICE,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -25,
        maxDamage = -80,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 15,
        maxDamage = 55,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Raaarrr!", yell = false},
    {text = "Aaaauuuuuooooooo!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 56000,
        maxCount = 52,
    },
    {
        id = "hailstorm rod",
        chance = 7400,
    },
    {
        id = "meat",
        chance = 52000,
        maxCount = 4,
    },
    {
        id = "wolf paw",
        chance = 3700,
    },
    {
        id = "shiver arrow",
        chance = 11000,
        maxCount = 10,
    },
    {
        id = "crystalline armor",
        chance = 3700,
    },
}

mtype:register(monster)
