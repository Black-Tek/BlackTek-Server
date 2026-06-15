local mtype = Game.createMonsterType("Yeti")
local monster = {}

monster.name = "Yeti"
monster.description = "a yeti"
monster.experience = 460
monster.race = "blood"
monster.maxHealth = 950
monster.health = 950
monster.speed = 250
monster.manaCost = 0
monster.corpse = 6038
monster.outfit = { lookType = 110 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -200,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -180,
        shootEffect = CONST_ANI_SNOWBALL,
        effect = CONST_ME_POFF,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 12,
        minDamage = 0,
        maxDamage = -175,
        length = 3,
        spread = 3,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 28,
    armor = 28,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -1},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Yooodelaaahooohooo!", yell = false},
    {text = "Yooodelaaaheeeheee!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 79310,
        maxCount = 27,
    },
    {
        id = 2666,
        chance = 71264,
    },
    {
        id = 2111,
        chance = 52874,
        maxCount = 22,
    },
    {
        id = 2671,
        chance = 39080,
    },
    {
        id = 2644,
        chance = 1149,
    },
    {
        id = 2129,
        chance = 1149,
    },
}

mtype:register(monster)
