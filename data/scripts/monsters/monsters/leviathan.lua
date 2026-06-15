local mtype = Game.createMonsterType("Leviathan")
local monster = {}

monster.name = "Leviathan"
monster.description = "Leviathan"
monster.experience = 5000
monster.race = "blood"
monster.maxHealth = 6000
monster.health = 6000
monster.speed = 758
monster.manaCost = 0
monster.corpse = 8307
monster.outfit = { lookType = 275 }
monster.runHealth = 600
monster.changeTarget = {
    interval = 2000,
    chance = 50,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
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
        maxDamage = -298,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 8,
        minDamage = -130,
        maxDamage = -460,
        length = 9,
        spread = 3,
        effect = CONST_ME_BIGPLANTS,
    },
    {
        name = "ice",
        interval = 1000,
        chance = 10,
        minDamage = -365,
        maxDamage = -491,
        length = 9,
        spread = 3,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "drown",
        interval = 1000,
        chance = 11,
        minDamage = -15,
        maxDamage = -20,
        radius = 4,
        target = true,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 30,
    armor = 20,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 50,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "CHHHRRRR", yell = false},
    {text = "HISSSS", yell = false},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 10000,
        maxCount = 3,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 80,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 80,
    },
    {
        id = "platinum coin",
        chance = 40000,
        maxCount = 6,
    },
}

mtype:register(monster)
