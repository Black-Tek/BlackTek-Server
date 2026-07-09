local mtype = Game.createMonsterType("Chizzoron The Distorter")
local monster = {}

monster.name = "Chizzoron The Distorter"
monster.description = "Chizzoron the Distorter"
monster.experience = 4000
monster.race = "blood"
monster.maxHealth = 16000
monster.health = 16000
monster.speed = 260
monster.manaCost = 0
monster.corpse = 11316
monster.outfit = { lookType = 340 }
monster.changeTarget = {
    interval = 2000,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -455,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        length = 8,
        spread = 3,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = -300,
        maxDamage = -646,
        radius = 3,
        effect = CONST_ME_POFF,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -148,
        maxDamage = -250,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 85,
    armor = 70,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Lizard Dragon Priest", interval = 2000, chance = 10, max = 2},
}
monster.voices = {
    interval = 2000,
    chance = 10,
    {text = "Humanzzz! Leave Zzaion at onzzzze!", yell = false},
    {text = "I pray to my mazzterzz, the mighty dragonzzz!", yell = false},
    {text = "You are not worzzy to touch zzizz zzacred ground!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 69825,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 69825,
        maxCount = 10,
    },
    {
        id = "small emerald",
        chance = 5750,
    },
    {
        id = "green gem",
        chance = 16300,
    },
    {
        id = 2169,
        chance = 11025,
    },
    {
        id = "dragon scale mail",
        chance = 5750,
    },
    {
        id = "lizard scale",
        chance = 100000,
    },
    {
        id = "great health potion",
        chance = 5750,
    },
    {
        id = "gold ingot",
        chance = 71550,
        maxCount = 2,
    },
}

mtype:register(monster)
