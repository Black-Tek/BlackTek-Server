local mtype = Game.createMonsterType("Snake Thing")
local monster = {}

monster.name = "Snake Thing"
monster.description = "Snake Thing"
monster.experience = 4600
monster.race = "venom"
monster.maxHealth = 20000
monster.health = 20000
monster.speed = 240
monster.manaCost = 0
monster.corpse = 12385
monster.outfit = { lookType = 220 }
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
        maxDamage = -400,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 35,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 40,
        length = 8,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 30,
        minDamage = -30,
        maxDamage = -600,
        radius = 6,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 30,
    armor = 45,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 10,
    {text = "POWER! I SEED MORE POWER!", yell = true},
}

mtype:register(monster)
