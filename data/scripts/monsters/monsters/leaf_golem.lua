local mtype = Game.createMonsterType("Leaf Golem")
local monster = {}

monster.name = "Leaf Golem"
monster.description = "a leaf golem"
monster.experience = 45
monster.race = "undead"
monster.maxHealth = 90
monster.health = 90
monster.speed = 140
monster.manaCost = 390
monster.corpse = 21358
monster.outfit = { lookType = 567 }
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
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -15,
        target = true,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        radius = 3,
        speed = -600,
        duration = 9000,
        effect = CONST_ME_SMALLPLANTS,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -1},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "*crackle*", yell = false},
    {text = "*swwwwishhhh*", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 86830,
        maxCount = 27,
    },
    {
        id = "small emerald",
        chance = 1230,
    },
    {
        id = "white mushroom",
        chance = 4940,
        maxCount = 3,
    },
    {
        id = "swampling club",
        chance = 4940,
    },
    {
        id = "dowser",
        chance = 15230,
    },
    {
        id = "fir cone",
        chance = 14350,
    },
}

mtype:register(monster)
