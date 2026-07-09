local mtype = Game.createMonsterType("Yakchal")
local monster = {}

monster.name = "Yakchal"
monster.description = "Yakchal"
monster.experience = 4400
monster.race = "blood"
monster.maxHealth = 5750
monster.health = 5750
monster.speed = 200
monster.manaCost = 0
monster.corpse = 20546
monster.outfit = {
    lookType = 149,
    lookHead = 9,
    lookBody = 0,
    lookLegs = 85,
    lookFeet = 85,
    lookAddons = 1,
}
monster.changeTarget = {
    interval = 2000,
    chance = 5,
}
monster.targetDistance = 4
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -389,
    },
    {
        name = "outfit",
        interval = 1000,
        chance = 10,
        range = 7,
        duration = 8000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 18,
        minDamage = 0,
        maxDamage = -430,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_SMALLICE,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 34,
        range = 7,
        minDamage = -200,
        maxDamage = -310,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_SNOWBALL,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 10,
        range = 7,
        speed = -600,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 20,
    armor = 15,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 50,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_HOLYDAMAGE, percent = 26},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Ice Golem", interval = 1000, chance = 13, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "So you think you are cool?", yell = false},
    {text = "I hope it is not too cold for you! HeHeHe.", yell = false},
    {text = "Freeze!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 10000,
        maxCount = 88,
    },
    {
        id = "boots of haste",
        chance = 2000,
    },
    {
        id = "dragon necklace",
        chance = 3250,
    },
    {
        id = "skull staff",
        chance = 4500,
    },
    {
        id = "crystal mace",
        chance = 4000,
    },
    {
        id = "blue piece of cloth",
        chance = 5000,
    },
    {
        id = "shard",
        chance = 100000,
    },
    {
        id = "queen's sceptre",
        chance = 1500,
    },
    {
        id = "berserk potion",
        chance = 1500,
    },
    {
        id = "mastermind potion",
        chance = 1500,
    },
    {
        id = "bullseye potion",
        chance = 1500,
    },
    {
        id = "crystal sword",
        chance = 4000,
    },
    {
        id = "pair of earmuffs",
        chance = 1000,
    },
    {
        id = "glacier kilt",
        chance = 2650,
    },
    {
        id = "glacier robe",
        chance = 3000,
    },
    {
        id = "gold ingot",
        chance = 5000,
    },
}

mtype:register(monster)
