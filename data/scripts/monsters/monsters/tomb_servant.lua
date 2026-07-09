local mtype = Game.createMonsterType("Tomb Servant")
local monster = {}

monster.name = "Tomb Servant"
monster.description = "a tomb servant"
monster.experience = 215
monster.race = "undead"
monster.maxHealth = 475
monster.health = 475
monster.speed = 190
monster.manaCost = 0
monster.corpse = 6029
monster.outfit = { lookType = 100 }
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
    illusionable = false,
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
        maxDamage = -130,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -40,
        maxDamage = -55,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Chaarr!!", yell = false},
    {text = "Ngl..Nglll...Gll", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 41000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 41000,
        maxCount = 8,
    },
    {
        id = "scarab coin",
        chance = 8210,
    },
    {
        id = "rotten meat",
        chance = 2000,
    },
    {
        id = 2230,
        chance = 49000,
    },
    {
        id = "longsword",
        chance = 6560,
    },
    {
        id = "bone shield",
        chance = 5300,
    },
    {
        id = "worm",
        chance = 25325,
        maxCount = 12,
    },
    {
        id = "half-digested piece of meat",
        chance = 1000,
    },
    {
        id = "fist on a stick",
        chance = 230,
    },
}

mtype:register(monster)
