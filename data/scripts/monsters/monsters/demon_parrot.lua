local mtype = Game.createMonsterType("Demon Parrot")
local monster = {}

monster.name = "Demon Parrot"
monster.description = "a demon parrot"
monster.experience = 225
monster.race = "blood"
monster.maxHealth = 360
monster.health = 360
monster.speed = 300
monster.manaCost = 250
monster.corpse = 6056
monster.outfit = { lookType = 217 }
monster.changeTarget = {
    interval = 5000,
    chance = 20,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "drunk",
        interval = 1000,
        chance = 30,
        length = 5,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 30,
        range = 5,
        minDamage = -25,
        maxDamage = -45,
        shootEffect = CONST_ANI_SUDDENDEATH,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 30,
        range = 1,
        minDamage = -15,
        maxDamage = -45,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "drunk", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "ISHH THAT THE BESHHT YOU HAVE TO OFFERRR?", yell = false},
    {text = "YOU ARRRRRE DOOMED!", yell = false},
    {text = "I SHHMELL FEEAARRR!", yell = false},
    {text = "MY SHHEED IS FEARRR AND MY HARRRVEST ISHH YOURRR SHHOUL!", yell = false},
    {text = "Your shhoooul will be mineee!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 99,
    },
}

mtype:register(monster)
