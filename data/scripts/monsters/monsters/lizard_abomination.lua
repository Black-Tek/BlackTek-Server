local mtype = Game.createMonsterType("Lizard Abomination")
local monster = {}

monster.name = "Lizard Abomination"
monster.description = "a lizard abomination"
monster.experience = 1350
monster.race = "blood"
monster.maxHealth = 20000
monster.health = 20000
monster.speed = 220
monster.manaCost = 0
monster.corpse = 12385
monster.outfit = { lookType = 364 }
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -550,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 40,
        radius = 3,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 50,
        minDamage = -200,
        maxDamage = -300,
        length = 8,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        radius = 3,
        speed = -800,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 60,
    armor = 55,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 10,
    {text = "NOOOO! NOW YOU HERETICS WILL FACE MY GODLY WRATH!", yell = true},
    {text = "RAAARRRR! I WILL DEVOL YOU!", yell = true},
    {text = "I WILL MAKE YOU ZHEE!", yell = true},
}

mtype:register(monster)
