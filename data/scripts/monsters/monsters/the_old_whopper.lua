local mtype = Game.createMonsterType("The Old Whopper")
local monster = {}

monster.name = "The Old Whopper"
monster.description = "The Old Whopper"
monster.experience = 750
monster.race = "blood"
monster.maxHealth = 785
monster.health = 785
monster.speed = 420
monster.manaCost = 0
monster.corpse = 7740
monster.outfit = { lookType = 277 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -175,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -170,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 50,
        duration = 34000,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 1},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Han oydar hot auden oydar", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "club ring",
        chance = 90,
    },
    {
        id = "battle axe",
        chance = 5450,
    },
    {
        id = "double axe",
        chance = 880,
    },
    {
        id = "battle hammer",
        chance = 5200,
    },
    {
        id = "heavy machete",
        chance = 2000,
    },
    {
        id = 2490,
        chance = 200,
    },
    {
        id = "plate shield",
        chance = 2000,
    },
    {
        id = "battle shield",
        chance = 6190,
    },
    {
        id = "meat",
        chance = 49950,
    },
    {
        id = 7398,
        chance = 75000,
    },
    {
        id = "spiked squelcher",
        chance = 5150,
    },
    {
        id = "strong health potion",
        chance = 390,
    },
    {
        id = "cyclops toe",
        chance = 10280,
    },
}

mtype:register(monster)
