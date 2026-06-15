local mtype = Game.createMonsterType("Boogey")
local monster = {}

monster.name = "Boogey"
monster.description = "Boogey"
monster.experience = 475
monster.race = "undead"
monster.maxHealth = 930
monster.health = 930
monster.speed = 400
monster.manaCost = 0
monster.corpse = 10324
monster.outfit = { lookType = 300 }
monster.changeTarget = {
    interval = 5000,
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
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -120,
    },
    {
        name = "physical",
        interval = 1500,
        chance = 30,
        range = 7,
        minDamage = 0,
        maxDamage = -30,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "death",
        interval = 1500,
        chance = 30,
        range = 7,
        minDamage = -12,
        maxDamage = -20,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 1500,
        chance = 40,
        minDamage = -20,
        maxDamage = -30,
        length = 8,
        spread = 3,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 50,
    armor = 50,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 80,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_ICEDAMAGE, percent = 25},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "demon skeleton", interval = 4000, chance = 40, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Since you didn't eat your spinach Bogey comes to get you!", yell = true},
    {text = "Too bad you did not eat your lunch, now I have to punish you!", yell = true},
    {text = "Even if you beat me, I'll hide in your closet until you one day drop your guard!", yell = true},
    {text = "You better had believe in me!", yell = true},
    {text = "I'll take you into the darkness ... forever!", yell = true},
}

mtype:register(monster)
