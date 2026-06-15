local mtype = Game.createMonsterType("Infernal Frog")
local monster = {}

monster.name = "Infernal Frog"
monster.description = "an infernal frog"
monster.experience = 190
monster.race = "blood"
monster.maxHealth = 655
monster.health = 655
monster.speed = 220
monster.manaCost = 0
monster.corpse = 6079
monster.outfit = {
    lookType = 224,
    lookHead = 69,
    lookBody = 66,
    lookLegs = 69,
    lookFeet = 66,
}
monster.runHealth = 40
monster.changeTarget = {
    interval = 4000,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -20,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 30,
        minDamage = -16,
        maxDamage = -32,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        speed = 400,
        duration = 8000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ribbit!", yell = false},
    {text = "Ribbit! Ribbit!", yell = false},
    {text = "No Kisses for you!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 77000,
        maxCount = 65,
    },
}

mtype:register(monster)
