local mtype = Game.createMonsterType("Shadow Hound")
local monster = {}

monster.name = "Shadow Hound"
monster.description = "a shadow hound"
monster.experience = 600
monster.race = "blood"
monster.maxHealth = 555
monster.health = 555
monster.speed = 360
monster.manaCost = 0
monster.corpse = 9923
monster.outfit = { lookType = 322 }
monster.changeTarget = {
    interval = 4000,
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
        maxDamage = -350,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 24,
        minDamage = -60,
        maxDamage = -160,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "monster soulfire",
        interval = 2000,
        chance = 14,
    },
}
monster.defenses = {
    defense = 55,
    armor = 50,
    {
        name = "healing",
        interval = 1000,
        chance = 15,
        minDamage = 60,
        maxDamage = 230,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrr!", yell = true},
}
monster.loot = {
    {
        id = "midnight shard",
        chance = 8333,
    },
}

mtype:register(monster)
