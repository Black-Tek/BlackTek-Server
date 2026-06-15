local mtype = Game.createMonsterType("Bones")
local monster = {}

monster.name = "Bones"
monster.description = "Bones"
monster.experience = 3750
monster.race = "undead"
monster.maxHealth = 9500
monster.health = 9500
monster.speed = 300
monster.manaCost = 0
monster.corpse = 6306
monster.outfit = { lookType = 231 }
monster.runHealth = 100
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -845,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 13,
        minDamage = -400,
        maxDamage = -600,
        radius = 1,
        target = true,
    },
    {
        name = "death",
        interval = 3000,
        chance = 34,
        range = 1,
        minDamage = -180,
        maxDamage = -500,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_DEATH,
    },
}
monster.defenses = {
    defense = 55,
    armor = 50,
    {
        name = "healing",
        interval = 5000,
        chance = 25,
        minDamage = 60,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Your new name is breakfast.", yell = false},
    {text = "Keep that dog away!", yell = false},
    {text = "Out Fluffy! Out! Bad dog!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 90,
    },
    {
        id = "sword ring",
        chance = 10000,
    },
    {
        id = "broadsword",
        chance = 4000,
    },
    {
        id = "magic plate armor",
        chance = 2000,
    },
    {
        id = "spectral stone",
        chance = 800,
    },
    {
        id = "skull helmet",
        chance = 50000,
    },
    {
        id = "soul orb",
        chance = 10000,
    },
    {
        id = 6300,
        chance = 4000,
    },
    {
        id = "demonic essence",
        chance = 1538,
    },
    {
        id = 6570,
        chance = 5538,
        maxCount = 3,
    },
    {
        id = 6571,
        chance = 1538,
    },
    {
        id = "dragonbone staff",
        chance = 50000,
    },
}

mtype:register(monster)
