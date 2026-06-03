local mtype = Game.createMonsterType("Acid Blob")
local monster = {}

monster.name = "Acid Blob"
monster.description = "an acid blob"
monster.experience = 250
monster.race = "venom"
monster.maxHealth = 250
monster.health = 250
monster.speed = 180
monster.manaCost = 0
monster.corpse = 9962
monster.outfit = { lookType = 314 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 30,
        minDamage = -10,
        maxDamage = -20,
        radius = 4,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -40,
        maxDamage = -60,
        length = 5,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Acid Blob", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Kzzchhh!", yell = false},
}
monster.loot = {
    {
        id = "glob of acid slime",
        chance = 14285,
    },
}

mtype:register(monster)
