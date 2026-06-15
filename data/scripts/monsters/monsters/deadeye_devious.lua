local mtype = Game.createMonsterType("Deadeye Devious")
local monster = {}

monster.name = "Deadeye Devious"
monster.description = "Deadeye Devious"
monster.experience = 750
monster.race = "blood"
monster.maxHealth = 1450
monster.health = 1450
monster.speed = 300
monster.manaCost = 0
monster.corpse = 20378
monster.outfit = {
    lookType = 151,
    lookHead = 115,
    lookBody = 76,
    lookLegs = 35,
    lookFeet = 117,
    lookAddons = 2,
}
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -550,
    },
    {
        name = "physical",
        interval = 4000,
        chance = 60,
        minDamage = -200,
        maxDamage = -250,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 50,
    armor = 35,
    {
        name = "healing",
        interval = 6000,
        chance = 65,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Let's kill 'em", yell = false},
    {text = "Arrrgh!", yell = false},
    {text = "You'll never take me alive!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 65,
    },
    {
        id = "skull of Ratha",
        chance = 100000,
        maxCount = 2,
    },
    {
        id = "plate armor",
        chance = 4000,
    },
    {
        id = "knight armor",
        chance = 1200,
    },
    {
        id = "meat",
        chance = 100000,
    },
    {
        id = "pirate backpack",
        chance = 4000,
    },
    {
        id = "Deadeye Devious' eye patch",
        chance = 100000,
    },
}

mtype:register(monster)
