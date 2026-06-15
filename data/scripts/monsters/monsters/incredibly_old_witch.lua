local mtype = Game.createMonsterType("Incredibly Old Witch")
local monster = {}

monster.name = "Incredibly Old Witch"
monster.description = "an incredibly old witch"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 202
monster.manaCost = 0
monster.corpse = 20535
monster.outfit = { lookType = 54 }
monster.changeTarget = {
    interval = 5000,
    chance = 20,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 12,
        range = 7,
        radius = 4,
        target = true,
        duration = 60000,
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
    {text = "Feel the wrath of the witch!", yell = false},
    {text = "Oh how you will regret to have distubed me!", yell = false},
    {text = "Everyone is so stupid!", yell = false},
    {text = "Stupid people!", yell = false},
    {text = "I will teach them all to leave me alone!", yell = false},
}

mtype:register(monster)
