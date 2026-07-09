local mtype = Game.createMonsterType("Ungreez")
local monster = {}

monster.name = "Ungreez"
monster.description = "Ungreez"
monster.experience = 500
monster.race = "fire"
monster.maxHealth = 8200
monster.health = 8200
monster.speed = 330
monster.manaCost = 0
monster.corpse = 5995
monster.outfit = { lookType = 35 }
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -480,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 13,
        range = 7,
        minDamage = 0,
        maxDamage = -110,
        shootEffect = CONST_ANI_SUDDENDEATH,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 14,
        range = 7,
        minDamage = -150,
        maxDamage = -250,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 18,
        minDamage = -200,
        maxDamage = -400,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 12,
        minDamage = -300,
        maxDamage = -380,
        length = 8,
        effect = CONST_ME_PURPLEENERGY,
    },
}
monster.defenses = {
    defense = 55,
    armor = 55,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 90,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 25},
    {type = COMBAT_ICEDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I'll teach you that even heros can die", yell = true},
    {text = "You wil die Begging like the others did", yell = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 21000,
        maxCount = 90,
    },
    {
        id = "fire mushroom",
        chance = 10000,
        maxCount = 6,
    },
    {
        id = "great mana potion",
        chance = 20000,
    },
    {
        id = "great health potion",
        chance = 20000,
    },
}

mtype:register(monster)
