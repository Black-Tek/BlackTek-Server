local mtype = Game.createMonsterType("Evil Mastermind")
local monster = {}

monster.name = "Evil Mastermind"
monster.description = "Evil Mastermind"
monster.experience = 675
monster.race = "undead"
monster.maxHealth = 1295
monster.health = 1295
monster.speed = 350
monster.manaCost = 0
monster.corpse = 10321
monster.outfit = { lookType = 256 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 3
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -77,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = -50,
        maxDamage = -78,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = -66,
        maxDamage = -72,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = -36,
        maxDamage = -57,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = -70,
        maxDamage = -73,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = -59,
        maxDamage = -75,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -600,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 50,
        maxDamage = 110,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 90},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "vampire", interval = 2000, chance = 40, max = 1},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You won't stop my masterplan to flood the world market with fake Bonelord language dictionaries!", yell = false},
    {text = "My calculations tell me you'll die!", yell = false},
    {text = "You can't stop me!", yell = false},
    {text = "Beware! My evil monolog is coming!", yell = false},
}

mtype:register(monster)
