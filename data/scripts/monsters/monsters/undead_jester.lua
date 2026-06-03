local mtype = Game.createMonsterType("Undead Jester")
local monster = {}

monster.name = "Undead Jester"
monster.description = "an undead jester"
monster.experience = 5
monster.race = "blood"
monster.maxHealth = 355
monster.health = 355
monster.speed = 224
monster.manaCost = 0
monster.corpse = 20522
monster.outfit = {
    lookType = 273,
    lookHead = 0,
    lookBody = 0,
    lookLegs = 114,
    lookFeet = 0,
    lookAddons = 2,
}
monster.runHealth = 55
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -3,
    },
    {
        name = "drunk",
        interval = 1000,
        chance = 20,
        range = 7,
        shootEffect = CONST_ANI_ENERGY,
    },
}
monster.defenses = {
    defense = 3,
    armor = 3,
    {
        name = "invisible",
        interval = 4000,
        chance = 30,
        duration = 2000,
        effect = CONST_ME_MORTAREA,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drunk", condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "cave rat", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Who's bad?", yell = false},
    {text = "I have a cunning plan!", yell = false},
    {text = "Resistance is futile! You will be amused!", yell = false},
    {text = "Pull my finger!", yell = false},
    {text = "Why did the chicken cross the road? TO KILL YOU!!!", yell = false},
    {text = "I will teach you all to mock me!", yell = false},
    {text = "He who laughs last, Laughs best!", yell = false},
    {text = "Th-Th-Th-That's all, folks!", yell = false},
    {text = "A zathroth priest, a druid and a paladin walk into a bar ...Ha Ha!", yell = false},
    {text = "Doh!", yell = false},
    {text = "Zathroth made me do it!", yell = false},
    {text = "And now for something completely different!", yell = false},
    {text = "You think this is funny now?", yell = false},
    {text = "Are we having fun yet?", yell = false},
    {text = "Did I do that?", yell = false},
}
monster.loot = {
    {
        id = 2667,
        chance = 60000,
    },
    {
        id = 9108,
        chance = 40000,
    },
    {
        id = 9696,
        chance = 1000,
    },
    {
        id = 9697,
        chance = 1000,
    },
    {
        id = 9698,
        chance = 1000,
    },
    {
        id = 9699,
        chance = 1000,
    },
    {
        id = 9695,
        chance = 800,
    },
    {
        id = 9694,
        chance = 400,
    },
    {
        id = "white piece of cloth",
        chance = 1400,
    },
    {
        id = "brown piece of cloth",
        chance = 1100,
    },
    {
        id = "green piece of cloth",
        chance = 1000,
    },
    {
        id = "rainbow trout",
        chance = 1000,
    },
    {
        id = "blue piece of cloth",
        chance = 900,
    },
    {
        id = "green perch",
        chance = 900,
    },
    {
        id = "yellow piece of cloth",
        chance = 700,
    },
    {
        id = "red piece of cloth",
        chance = 500,
    },
    {
        id = "marlin",
        chance = 200,
    },
}

mtype:register(monster)
