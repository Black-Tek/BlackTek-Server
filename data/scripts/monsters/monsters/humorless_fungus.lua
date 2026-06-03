local mtype = Game.createMonsterType("Humorless Fungus")
local monster = {}

monster.name = "Humorless Fungus"
monster.description = "a humorless fungus"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 2500
monster.health = 2500
monster.speed = 230
monster.manaCost = 0
monster.corpse = 17428
monster.outfit = { lookType = 517 }
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
        maxDamage = -475,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -40,
        maxDamage = -197,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -525,
        target = true,
        shootEffect = CONST_ANI_SNOWBALL,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -400,
        maxDamage = -640,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 4,
        target = true,
        duration = 4000,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 0,
        maxDamage = 230,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Munch munch munch!", yell = false},
    {text = "Chatter", yell = false},
}

mtype:register(monster)
