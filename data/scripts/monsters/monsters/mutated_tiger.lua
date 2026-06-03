local mtype = Game.createMonsterType("Mutated Tiger")
local monster = {}

monster.name = "Mutated Tiger"
monster.description = "a mutated tiger"
monster.experience = 750
monster.race = "blood"
monster.maxHealth = 1100
monster.health = 1100
monster.speed = 240
monster.manaCost = 0
monster.corpse = 9913
monster.outfit = { lookType = 318 }
monster.runHealth = 100
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = -200,
        length = 5,
        spread = 3,
        effect = CONST_ME_BLOCKHIT,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 150,
        maxDamage = 300,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = 80},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "GRAAARRRRRR", yell = false},
    {text = "CHHHHHHHHHHH", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 5,
    },
    {
        id = "life ring",
        chance = 5580,
    },
    {
        id = "guardian shield",
        chance = 380,
    },
    {
        id = "meat",
        chance = 29500,
        maxCount = 2,
    },
    {
        id = "angelic axe",
        chance = 440,
    },
    {
        id = "glorious axe",
        chance = 870,
    },
    {
        id = "strong health potion",
        chance = 6000,
    },
    {
        id = "silky tapestry",
        chance = 730,
    },
    {
        id = "striped fur",
        chance = 20130,
    },
    {
        id = "sabretooth",
        chance = 10600,
    },
}

mtype:register(monster)
