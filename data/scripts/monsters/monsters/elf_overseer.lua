local mtype = Game.createMonsterType("Elf Overseer")
local monster = {}

monster.name = "Elf Overseer"
monster.description = "an elf overseer"
monster.experience = 290
monster.race = "blood"
monster.maxHealth = 500
monster.health = 500
monster.speed = 176
monster.manaCost = 0
monster.corpse = 20596
monster.outfit = {
    lookType = 159,
    lookHead = 21,
    lookBody = 76,
    lookLegs = 95,
    lookFeet = 116,
}
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -75,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 10,
        minDamage = -50,
        maxDamage = -90,
        length = 6,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 10,
        range = 4,
        minDamage = -40,
        maxDamage = -50,
        radius = 2,
        target = true,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 50,
        maxDamage = 72,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Do YOU want some more water, yes? YES?", yell = false},
    {text = "DOWN WITH THE FIRES OF IGNORANCE!!", yell = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 51000,
        maxCount = 50,
    },
    {
        id = "elvish talisman",
        chance = 20000,
    },
    {
        id = "bread",
        chance = 14000,
        maxCount = 3,
    },
    {
        id = "health potion",
        chance = 14000,
    },
    {
        id = "battle shield",
        chance = 11000,
    },
    {
        id = "holy orchid",
        chance = 8500,
    },
    {
        id = "mana potion",
        chance = 8500,
    },
    {
        id = "crystal sword",
        chance = 5700,
    },
}

mtype:register(monster)
