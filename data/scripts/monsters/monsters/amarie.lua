local mtype = Game.createMonsterType("Amarie")
local monster = {}

monster.name = "Amarie"
monster.description = "Amarie"
monster.experience = 42
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 190
monster.manaCost = 0
monster.corpse = 20593
monster.outfit = {
    lookType = 159,
    lookHead = 70,
    lookBody = 16,
    lookLegs = 9,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -15,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -25,
        shootEffect = CONST_ANI_ARROW,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Aaaaah! Let me out of here!!", yell = false},
    {text = "Help! Where am I?", yell = false},
    {text = "Noooo! What are you doing?", yell = false},
    {text = "Ulathil beia Thratha!", yell = false},
    {text = "What is this devilish scheme??", yell = false},
}

mtype:register(monster)
