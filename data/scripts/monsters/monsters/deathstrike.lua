local mtype = Game.createMonsterType("Deathstrike")
local monster = {}

monster.name = "Deathstrike"
monster.description = "Deathstrike"
monster.experience = 40000
monster.race = "undead"
monster.maxHealth = 200000
monster.health = 200000
monster.speed = 470
monster.manaCost = 0
monster.corpse = 18384
monster.outfit = { lookType = 500 }
monster.runHealth = 10000
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
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
        interval = 1300,
        minDamage = 0,
        maxDamage = -540,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        speed = -400,
        duration = 2500,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 100,
        range = 7,
        minDamage = -820,
        maxDamage = -950,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 12,
        range = 3,
        minDamage = -350,
        maxDamage = -800,
        target = true,
        effect = CONST_ME_PURPLEENERGY,
    },
}
monster.defenses = {
    defense = 35,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 1000,
        maxDamage = 5500,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -20},
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Deeestructiooon!", yell = false},
    {text = "Maaahhhrrr!", yell = false},
    {text = "You are nothing!", yell = false},
    {text = "Taaake ... this!", yell = false},
}
monster.loot = {
    {
        id = "Deathstrike's snippet",
        chance = 100000,
    },
    {
        id = "crystalline sword",
        chance = 10480,
    },
    {
        id = "crystalline axe",
        chance = 11430,
    },
    {
        id = "mycological mace",
        chance = 8570,
    },
    {
        id = "crystal crossbow",
        chance = 12380,
    },
    {
        id = "mycological bow",
        chance = 11430,
    },
    {
        id = "shiny blade",
        chance = 11430,
    },
}

mtype:register(monster)
