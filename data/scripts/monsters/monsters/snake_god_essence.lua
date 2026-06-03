local mtype = Game.createMonsterType("Snake God Essence")
local monster = {}

monster.name = "Snake God Essence"
monster.description = "Snake God Essence"
monster.experience = 1350
monster.race = "blood"
monster.maxHealth = 20000
monster.health = 20000
monster.speed = 300
monster.manaCost = 0
monster.corpse = 12385
monster.outfit = { lookType = 356 }
monster.changeTarget = {
    interval = 2000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
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
        maxDamage = -800,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 40,
        length = 8,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 50,
        minDamage = -150,
        maxDamage = -270,
        radius = 6,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 65,
    armor = 70,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 10,
    {text = "AHHH ZHE POWER...", yell = true},
    {text = "ZHE TIME OF ZHE SNAKE HAZ COME!", yell = true},
}

mtype:register(monster)
