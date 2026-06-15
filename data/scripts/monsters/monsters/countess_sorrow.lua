local mtype = Game.createMonsterType("Countess Sorrow")
local monster = {}

monster.name = "Countess Sorrow"
monster.description = "Countess Sorrow"
monster.experience = 13000
monster.race = "undead"
monster.maxHealth = 6500
monster.health = 6500
monster.speed = 250
monster.manaCost = 0
monster.corpse = 6344
monster.outfit = {
    lookType = 241,
    lookHead = 20,
}
monster.changeTarget = {
    interval = 60000,
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
        maxDamage = -360,
        -- NOTE: melee condition (poison=50); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 50000,
        },
    },
    {
        name = "earth",
        interval = 1000,
        chance = 10,
        range = 7,
        minDamage = -150,
        maxDamage = -350,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisonfield",
        interval = 1000,
        chance = 17,
        range = 7,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "manadrain",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -100,
        maxDamage = -400,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 20,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 50,
        minDamage = 409,
        maxDamage = 812,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Phantasm", interval = 2000, chance = 20, max = 0},
    {name = "Phantasm Summon", interval = 2000, chance = 20, max = 0},
}
monster.loot = {
    {
        id = "Countess Sorrow's frozen tear",
        chance = 100000,
    },
}

mtype:register(monster)
