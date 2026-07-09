local mtype = Game.createMonsterType("Enraged Crystal Golem")
local monster = {}

monster.name = "Enraged Crystal Golem"
monster.description = "enraged crystal golem"
monster.experience = 550
monster.race = "energy"
monster.maxHealth = 700
monster.health = 700
monster.speed = 200
monster.manaCost = 0
monster.corpse = 18466
monster.outfit = { lookType = 508 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
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
        maxDamage = -150,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Crrrrk! Chhhhr!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "crystal sword",
        chance = 4710,
    },
    {
        id = "glorious axe",
        chance = 360,
    },
    {
        id = "strong health potion",
        chance = 7140,
    },
    {
        id = "strong mana potion",
        chance = 8270,
    },
    {
        id = "blue crystal splinter",
        chance = 4120,
    },
    {
        id = "cyan crystal fragment",
        chance = 1050,
    },
    {
        id = "crystalline spikes",
        chance = 7980,
    },
    {
        id = "crystalline axe",
        chance = 5008,
        maxCount = 5,
    },
}

mtype:register(monster)
