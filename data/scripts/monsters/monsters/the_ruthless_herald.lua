local mtype = Game.createMonsterType("The Ruthless Herald")
local monster = {}

monster.name = "The Ruthless Herald"
monster.description = "The Ruthless Herald"
monster.experience = 0
monster.race = "fire"
monster.maxHealth = 100
monster.health = 100
monster.speed = 210
monster.manaCost = 0
monster.corpse = 6364
monster.outfit = { lookType = 237 }
monster.changeTarget = {
    interval = 2000,
    chance = 95,
}
monster.targetDistance = 4
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "outfit",
        interval = 2000,
        chance = 25,
        radius = 3,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 25,
        radius = 3,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 25,
        radius = 3,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 25,
        radius = 3,
        target = true,
        duration = 6000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "holy", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "manadrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I am so proud of my son Orshabaal.", yell = false},
    {text = "Have you heard, the imperor is marrying an imp named April? What a fool.", yell = false},
    {text = "The Ruthless Seven are going to make the easter bunny an honorary member!", yell = false},
    {text = "They are coming ... perhaps for YOU!", yell = false},
    {text = "Beware! The Ruthless Seven are coming!", yell = false},
    {text = "Killing me is imp-ossible, because I am imp-roved! I am immune to any imp-act!", yell = false},
    {text = "The one who kills me gets an imp-outfit.", yell = false},
    {text = "Hey you! I've heard that! You're first to die when the masters come.", yell = false},
    {text = "Nice to meet you. I am Harold, the ruthless herald.", yell = false},
    {text = "Have you seen my friend Harvey? I could swear he's somewhere around.", yell = false},
    {text = "My masters are on their way!", yell = false},
}

mtype:register(monster)
