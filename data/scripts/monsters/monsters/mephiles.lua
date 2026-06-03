local mtype = Game.createMonsterType("Mephiles")
local monster = {}

monster.name = "Mephiles"
monster.description = "Mephiles"
monster.experience = 415
monster.race = "blood"
monster.maxHealth = 415
monster.health = 415
monster.speed = 300
monster.manaCost = 0
monster.corpse = 10323
monster.outfit = { lookType = 237 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 3
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
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -35,
    },
    {
        name = "fire",
        interval = 1500,
        chance = 70,
        range = 7,
        minDamage = -15,
        maxDamage = -45,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
    {
        name = "speed",
        interval = 1000,
        chance = 40,
        speed = 400,
        duration = 40000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 4000,
        chance = 50,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I have a contract here which you should sign!", yell = false},
    {text = "I sence so much potential in you. It's almost a shame I have to kill you.", yell = false},
    {text = "Yes, slay me for the loot I might have. Give in to your greed.", yell = false},
    {text = "Wealth, Power, it is all at your fingertips. All you have to do is a bit blackmailing and bullying.", yell = false},
    {text = "Come on. being a bit evil won't hurt you.", yell = false},
}

mtype:register(monster)
