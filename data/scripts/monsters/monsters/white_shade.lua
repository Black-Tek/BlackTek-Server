local mtype = Game.createMonsterType("White Shade")
local monster = {}

monster.name = "White Shade"
monster.description = "a white shade"
monster.experience = 120
monster.race = "undead"
monster.maxHealth = 260
monster.health = 260
monster.speed = 190
monster.manaCost = 0
monster.corpse = 21376
monster.outfit = { lookType = 560 }
monster.runHealth = 30
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
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
        maxDamage = -8,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -4,
        maxDamage = -6,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_YALAHARIGHOST,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 5,
        radius = 4,
        target = false,
        speed = -400,
        duration = 3000,
        effect = CONST_ME_HITAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "invisible",
        interval = 2000,
        chance = 5,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 90},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Leave this place... save yourself...", yell = false},
    {text = "The dark ones.. must be stopped... unholy, twisted, EVIL!", yell = false},
    {text = "No... don't go further stranger... danger lies in every step...", yell = false},
}
monster.loot = {
    {
        id = "white piece of cloth",
        chance = 3370,
    },
}

mtype:register(monster)
