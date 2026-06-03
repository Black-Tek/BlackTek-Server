local mtype = Game.createMonsterType("Gazer")
local monster = {}

monster.name = "Gazer"
monster.description = "a gazer"
monster.experience = 90
monster.race = "venom"
monster.maxHealth = 120
monster.health = 120
monster.speed = 140
monster.manaCost = 0
monster.corpse = 6036
monster.outfit = { lookType = 109 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -15,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -25,
        maxDamage = -35,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -10,
        maxDamage = -35,
    },
}
monster.defenses = {
    defense = 4,
    armor = 4,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Mommy!?", yell = false},
    {text = "Buuuuhaaaahhaaaaa!", yell = false},
    {text = "Me need mana!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 99000,
        maxCount = 24,
    },
    {
        id = "small flask of eyedrops",
        chance = 3330,
    },
}

mtype:register(monster)
