local mtype = Game.createMonsterType("Herald of Gloom")
local monster = {}

monster.name = "Herald of Gloom"
monster.description = "a herald of gloom"
monster.experience = 450
monster.race = "undead"
monster.maxHealth = 350
monster.health = 350
monster.speed = 260
monster.manaCost = 0
monster.corpse = 9915
monster.outfit = { lookType = 320 }
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
    {
        name = "speed",
        interval = 3000,
        chance = 10,
        range = 7,
        target = true,
        speed = -600,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "holy",
        interval = 2000,
        chance = 24,
        range = 4,
        minDamage = -90,
        maxDamage = -170,
        shootEffect = CONST_ANI_SMALLHOLY,
    },
}
monster.defenses = {
    defense = 55,
    armor = 50,
    {
        name = "speed",
        interval = 1000,
        chance = 15,
        speed = 200,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 5000,
        chance = 20,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 20,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "The powers of light are waning.", yell = true},
    {text = "You will join us in eternal night!", yell = true},
    {text = "The shadows will engulf the world.", yell = true},
}
monster.loot = {
    {
        id = "midnight shard",
        chance = 1886,
    },
}

mtype:register(monster)
