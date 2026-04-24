// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.

#include "otpch.h"
#include <toml++/toml.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "augments.h"
#include "combat.h"


namespace BlackTek
{
    static gtl::node_hash_map<std::string, std::shared_ptr<Augment>> global_augments{};

    std::shared_ptr<Augment> Augments::MakeAugment(std::string_view augmentName)
    {
        auto it = global_augments.find(augmentName.data());

        if (it != global_augments.end())
        {
            auto augmentClone = Augment::MakeAugment(*it->second);
            return augmentClone;
        }
        std::cout << "Failed to find augment named : " << augmentName;
        return nullptr;
    }

    void Augments::loadAll() 
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".toml")
            {
                try
                {
                    auto file = toml::parse_file(entry.path().string());

                    for (const auto& [index, entry] : file)
                    {

                        toml::table augment_info = *entry.as_table();
                        auto modifier_data = augment_info["modifiers"];
                        std::string name = augment_info["name"].value_or("unknown");
                        std::string description = augment_info["description"].value_or("unknown");

                        if (name == "unknown") 
                        {
                            std::cout << "Error: All augments require a name \n";
                            break;
                        }
                        std::shared_ptr<Augment> augment = Augment::MakeAugment(name, description);

                        if (auto mod_list = modifier_data.as_array())
                        {
                            mod_list->for_each([augment, name](auto&& prop) {
                                if (prop.is_table())
                                {
                                    auto& table = *prop.as_table();
                                    std::string_view modType = table["mod"].value_or("none");
                                    uint16_t amount = table["value"].value_or(0);
                                    std::string_view factor = table["factor"].value_or("none");
                                    uint8_t chance = table["chance"].value_or(100);
                                    std::string_view damageType = table["damage"].value_or("none");
                                    std::string_view originType = table["origin"].value_or("none");
                                    std::string_view creatureType = table["target"].value_or("none");
                                    std::string_view race = table["race"].value_or("none");
                                    std::string_view creatureName = table["monster"].value_or("none");

                                    // To-do: Change all static methods used below to accept const values and use const variables above.
                                    // also change the 'Get' methods into 'parse' methods for clarity
                                    if (ParseStance(modType) == std::to_underlying(DamageModifier::Stance::Attack))
                                    {
                                        auto damage_modifier = DamageModifier(
                                            ParseStance(modType),
                                            ParseAttackModifier(modType),
                                            amount,
                                            ParseFactor(factor),
                                            chance,
                                            ParseDamage(damageType),
                                            ParseOrigin(originType),
                                            ParseCreatureType(creatureType),
                                            ParseRaceType(race));

                                        // To-do : create a new variable for storing monster names to not conflict with other aux variables
                                        if (modType == "conversion") 
                                        {
                                            auto convertedType = ParseDamage(table["toDamage"].value_or("none"));
                                            damage_modifier.setTransformDamageType(convertedType);
                                        }

                                        if (creatureName != "none")
                                            damage_modifier.setCreatureName(creatureName);

                                        augment->addModifier(std::move(damage_modifier));

                                    }
                                    else if (ParseStance(modType) == std::to_underlying(DamageModifier::Stance::Defense))
                                    {
                                        auto damage_modifier = DamageModifier(
                                            ParseStance(modType),
                                            ParseDefenseModifier(modType),
                                            amount,
                                            ParseFactor(factor),
                                            chance,
                                            ParseDamage(damageType),
                                            ParseOrigin(originType),
                                            ParseCreatureType(creatureType),
                                            ParseRaceType(race));

                                        if (modType == "reform")
                                        {
                                            auto reformType = ParseDamage(table["toDamage"].value_or("none"));
                                            damage_modifier.setTransformDamageType(reformType);
                                        }

                                        if (creatureName != "none")
                                            damage_modifier.setCreatureName(creatureName);

                                        augment->addModifier(std::move(damage_modifier));
                                    }
                                    else
                                    {

                                        std::cout << "Modifier has unknown stance " << table["stance"] << "\n";
                                    }
                                }
                                });
                        }
                        AddAugment(augment);
                    }
                }
                catch (const toml::parse_error& err) 
                {
                    // todo handle through logger
                    std::cerr << "Error parsing file " << entry.path() << ": " << err << "\n";
                }
            }
        }
    }

    void Augments::clearAll()
    {
        global_augments.clear();
    }

    void Augments::reload()
    {
        clearAll();
        loadAll();
        // if (config::deleteOldAugments) { CleanPlayerAugments(); }
    }

    size_t Augments::count()
    {
        return global_augments.size();
    }

    const uint8_t Augments::ParseStance(std::string_view modName) noexcept
    {
        if (ParseAttackModifier(modName) != std::to_underlying(DamageModifier::AttackType::None))
        {
            return std::to_underlying(DamageModifier::Stance::Attack);
        }
        else if (ParseDefenseModifier(modName) != std::to_underlying(DamageModifier::DefenseType::None))
        {
            return std::to_underlying(DamageModifier::Stance::Defense);
        }
        std::cout << "[::Augment Error::] no such mod by type name : " << std::string{ modName } << " /n";
        return 0;
    }

    const uint8_t Augments::ParseFactor(std::string_view factor) noexcept
    {
        std::string f_type = std::string{ factor };

        if (f_type == "flat") 
            return std::to_underlying(DamageModifier::Factor::Flat);

        return std::to_underlying(DamageModifier::Factor::Percent);
    }

    const uint16_t Augments::ParseDamage(std::string_view damageName) noexcept
    {   // Note : If you add values to the list you must increase the size manually
        // current size is : 21
        const std::array<std::pair<std::string_view, Combat::DamageType>, 21> static_map { 
        {
            {"none",            Combat::DamageType::Unknown},
            {"all",             Combat::DamageType::Unknown},
            {"physical",        Combat::DamageType::Physical},
            {"melee",           Combat::DamageType::Physical},
            {"energy",          Combat::DamageType::Energy},
            {"electric",        Combat::DamageType::Energy},
            {"earth",           Combat::DamageType::Earth},
            {"poison",          Combat::DamageType::Earth},
            {"fire",            Combat::DamageType::Fire},
            {"lifedrain",       Combat::DamageType::LifeDrain},
            {"lifesteal",       Combat::DamageType::LifeDrain},
            {"lifeleech",       Combat::DamageType::LifeDrain},
            {"manadrain",       Combat::DamageType::ManaDrain},
            {"manasteal",       Combat::DamageType::ManaDrain},
            {"manaleech",       Combat::DamageType::ManaDrain},
            {"drown",           Combat::DamageType::Water},
            {"water",           Combat::DamageType::Water},
            {"ice",             Combat::DamageType::Ice},
            {"holy",            Combat::DamageType::Holy},
            {"death",           Combat::DamageType::Death},
            {"curse",           Combat::DamageType::Death},
        } };

        for (const auto& [key, value] : static_map) 
        {
            if (key == damageName) 
                return value;
        }

        return COMBAT_NONE;
    }

    const uint8_t Augments::ParseOrigin(std::string_view originName) noexcept
    {   // Note : If you add values to the list you must increase the size manually
        // current size is : 14
        const std::array<std::pair<std::string_view, uint8_t>, 13> static_map { 
        {
            {"none",            0},
            {"all",             0},
            {"condition",       1},
            {"spell",           2},
            {"melee",           3},
            {"ranged",          4},
            {"absorb",          5},
            {"restore",         6},
            {"reflect",         7},
            {"deflect",         8},
            {"ricochet",        9},
            {"piercing",        10},
            {"augment",         11},
        } };

        for (const auto& [key, value] : static_map) 
        {
            if (key == originName)
                return value;
        }

        return 0;
    }

    const uint8_t Augments::ParseAttackModifier(std::string_view modName) noexcept
    {
        // Note : If you add values to the list you must increase the size manually
        // current size is : 8
        const std::array<std::pair<std::string_view, DamageModifier::AttackType>, 8> static_map { 
        {
            {"none",            DamageModifier::AttackType::None},
            {"lifesteal",       DamageModifier::AttackType::Lifesteal},
            {"manasteal",       DamageModifier::AttackType::Manasteal},
            {"staminasteal",    DamageModifier::AttackType::Staminasteal},
            {"soulsteal",       DamageModifier::AttackType::Soulsteal},
            {"critical",        DamageModifier::AttackType::Critical},
            {"piercing",        DamageModifier::AttackType::Piercing},
            {"conversion",      DamageModifier::AttackType::Conversion},
        } };

        for (const auto& [key, value] : static_map) 
        {
            if (key == modName)
                return std::to_underlying(value);
        }

        return std::to_underlying(DamageModifier::AttackType::None);
    }

    const uint8_t Augments::ParseDefenseModifier(std::string_view modName) noexcept
    {   // Note : If you add values to the list you must increase the size manually
        // current size is : 11
        const std::array<std::pair<std::string_view, DamageModifier::DefenseType>, 11> static_map { 
        {
            {"none",            DamageModifier::DefenseType::None},
            {"absorb",          DamageModifier::DefenseType::Absorb},
            {"restore",         DamageModifier::DefenseType::Restore},
            {"replenish",       DamageModifier::DefenseType::Replenish},
            {"revive",          DamageModifier::DefenseType::Revive},
            {"reflect",         DamageModifier::DefenseType::Reflect},
            {"deflect",         DamageModifier::DefenseType::Deflect},
            {"ricochet",        DamageModifier::DefenseType::Ricochet},
            {"resist",          DamageModifier::DefenseType::Resist},
            {"reform",          DamageModifier::DefenseType::Reform},
            {"weakness",        DamageModifier::DefenseType::Weakness},
        } };

        for (const auto& [key, value] : static_map) 
        {
            if (key == modName)
                return std::to_underlying(value);
        }

        return std::to_underlying(DamageModifier::DefenseType::None);
    }


    const uint8_t Augments::ParseRaceType(std::string_view raceType) noexcept 
    {
        // Note : If you add values to the list you must increase the size manually
        // current size is : 6
        const std::array<std::pair<std::string_view, RaceType_t>, 6> static_map { 
        {
            {"none",            RACE_NONE},
            {"venom",           RACE_VENOM},
            {"blood",           RACE_BLOOD},
            {"undead",          RACE_UNDEAD},
            {"fire",            RACE_FIRE},
            {"energy",          RACE_ENERGY},
        } };

        for (const auto& [key, value] : static_map)
        {
            if (key == raceType)
                return value;
        }

        return RACE_NONE;
    }

    const uint8_t Augments::ParseCreatureType(std::string_view creatureType) noexcept 
    {
        // Note : If you add values to the list you must increase the size manually
        // current size is : 14
        const std::array<std::pair<std::string_view, CreatureType_t>, 14> static_map { 
        {
            {"player",                      CREATURETYPE_PLAYER},
            {"monster",                     CREATURETYPE_MONSTER},
            {"npc",                         CREATURETYPE_NPC},
            {"allsummon",                   CREATURETYPE_SUMMON_ALL},
            {"summons",                     CREATURETYPE_SUMMON_ALL},
            {"ownedsummon",                 CREATURETYPE_SUMMON_OWN},
            {"mysummon",                    CREATURETYPE_SUMMON_OWN},
            {"hostilesummon",               CREATURETYPE_SUMMON_HOSTILE},
            {"enemysummon",                 CREATURETYPE_SUMMON_HOSTILE},
            {"guildsummon",                 CREATURETYPE_SUMMON_GUILD},
            {"partysummon",                 CREATURETYPE_SUMMON_PARTY},
            {"boss",                        CREATURETYPE_BOSS},
            {"none",                        CREATURETYPE_ATTACKABLE},
            {"all",                         CREATURETYPE_ATTACKABLE},
        } };

        for (const auto& [key, value] : static_map) 
        {
            if (key == creatureType)
                return value;
        }

        return CREATURETYPE_ATTACKABLE;
    }

    void Augments::AddAugment(std::shared_ptr<BlackTek::Augment> augment)
    {
        auto [it, inserted] = global_augments.try_emplace(augment->getName().data(), augment);

        if (not inserted)
        {
            std::cout << "[Warning][Augments] " << augment->getName() << " already exists! \n";
        }
    }

    void Augments::RemoveAugment(const std::shared_ptr<BlackTek::Augment>& augment)
    {
        auto it = global_augments.find(augment->getName().data());
        if (it != global_augments.end())
            global_augments.erase(it);
    }

    void Augments::RemoveAugment(std::string_view augName)
    {
        auto it = global_augments.find(std::string(augName));
        if (it != global_augments.end())
            global_augments.erase(it);
    }

    void Augments::RemoveAugment(const std::string& augName)
    {
        auto it = global_augments.find(augName);
        if (it != global_augments.end())
            global_augments.erase(it);
    }

    std::shared_ptr<BlackTek::Augment> Augments::GetAugment(std::string_view augName)
    {
        auto it = global_augments.find(augName.data());
        if (it != global_augments.end())
        {
            auto augment = Augment::MakeAugment(*it->second);
            return augment;
        }
        return nullptr;
    }
}