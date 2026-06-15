local mtype = Game.createMonsterType("Draken Elite")
local monster = {}

monster.name = "Draken Elite"
monster.description = "a draken elite"
monster.experience = 4200
monster.race = "blood"
monster.maxHealth = 5550
monster.health = 5550
monster.speed = 250
monster.manaCost = 0
monster.corpse = 12609
monster.outfit = { lookType = 362 }
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -354,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -240,
        maxDamage = -550,
        length = 4,
        spread = 3,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -200,
        maxDamage = -300,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -280,
        maxDamage = -410,
        radius = 4,
        target = true,
        effect = CONST_ME_POFF,
    },
    {
        name = "monster soulfire",
        interval = 2000,
        chance = 10,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -250,
        maxDamage = -320,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 510,
        maxDamage = 600,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 40},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "For ze emperor!", yell = false},
    {text = "You will die zhouzandz deazhz!", yell = false},
}
monster.loot = {
    {
        id = "small diamond",
        chance = 2440,
        maxCount = 4,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 47000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 50360,
        maxCount = 8,
    },
    {
        id = "meat",
        chance = 30175,
    },
    {
        id = "magic sulphur",
        chance = 2100,
    },
    {
        id = "assassin dagger",
        chance = 980,
    },
    {
        id = "great mana potion",
        chance = 9340,
        maxCount = 3,
    },
    {
        id = "ultimate health potion",
        chance = 9250,
        maxCount = 3,
    },
    {
        id = "Zaoan armor",
        chance = 490,
    },
    {
        id = "Zaoan helmet",
        chance = 150,
    },
    {
        id = "Zaoan legs",
        chance = 770,
    },
    {
        id = "Zaoan sword",
        chance = 490,
    },
    {
        id = "elite draken mail",
        chance = 110,
    },
    {
        id = "twiceslicer",
        chance = 910,
    },
    {
        id = "draken sulphur",
        chance = 7600,
    },
    {
        id = "draken wristbands",
        chance = 14030,
    },
    {
        id = "broken draken mail",
        chance = 16930,
    },
    {
        id = "broken slicer",
        chance = 24670,
    },
    {
        id = "cobra crown",
        chance = 10,
    },
    {
        id = "draken boots",
        chance = 600,
    },
    {
        id = "snake god's wristguard",
        chance = 80,
    },
    {
        id = "blade of corruption",
        chance = 20,
    },
}

mtype:register(monster)
