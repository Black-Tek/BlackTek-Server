local mtype = Game.createMonsterType("Diblis The Fair")
local monster = {}

monster.name = "Diblis The Fair"
monster.description = "Diblis The Fair"
monster.experience = 1800
monster.race = "undead"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 280
monster.manaCost = 0
monster.corpse = 8937
monster.outfit = { lookType = 287 }
monster.changeTarget = {
    interval = 5000,
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
        maxDamage = -380,
    },
    {
        name = "death",
        interval = 1000,
        chance = 12,
        minDamage = 0,
        maxDamage = -155,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 1000,
        chance = 12,
        minDamage = 100,
        maxDamage = 235,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 3000,
        chance = 25,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -15},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Banshee", interval = 4500, chance = 50, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I envy you to be slain by someone as beautiful as me.", yell = false},
    {text = "I will drain your ugly corpses of the last drop of blood.", yell = false},
}
monster.loot = {
    {
        id = "black pearl",
        chance = 8900,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 99,
    },
    {
        id = "platinum coin",
        chance = 50000,
        maxCount = 5,
    },
    {
        id = "ring of healing",
        chance = 14111,
    },
    {
        id = 2229,
        chance = 15000,
    },
    {
        id = "vampire shield",
        chance = 2100,
    },
    {
        id = "strong health potion",
        chance = 1500,
    },
    {
        id = "spellbook of lost souls",
        chance = 300,
    },
    {
        id = "vampire lord token",
        chance = 100000,
    },
    {
        id = "scum bag",
        chance = 500,
    },
}

mtype:register(monster)
