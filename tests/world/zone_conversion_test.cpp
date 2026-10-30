#include "otpch.h"
#include "configmanager.h"
#include "map.h"
#include "iomap.h"
#include "zones.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

extern ConfigManager g_config;

namespace BlackTek::World::Tests
{
    class ZoneConversion : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            original_path = std::filesystem::current_path();
            test_path = std::filesystem::temp_directory_path() / ("blacktek-zone-conversion-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
            std::filesystem::create_directories(test_path);
            std::filesystem::current_path(test_path);
            Zones::ZoneManager::Clear();
            const auto& mapName = g_config.GetString(ConfigManager::MAP_NAME);
            output_root = std::filesystem::path("data/world/converted") / mapName;
            zones_path = output_root / (mapName + "-zones");
            std::filesystem::create_directories(zones_path);
            std::ofstream legacy("spawns.xml");
            legacy << "<spawns><spawn centerx=\"100\" centery=\"100\" centerz=\"7\"><monster name=\"Rat\" x=\"0\" y=\"0\" spawntime=\"60\"/></spawn></spawns>";
        }

        void TearDown() override
        {
            Zones::ZoneManager::Clear();
            std::filesystem::current_path(original_path);
            std::filesystem::remove_all(test_path);
        }

        std::filesystem::path original_path;
        std::filesystem::path test_path;
        std::filesystem::path output_root;
        std::filesystem::path zones_path;
    };

    TEST_F(ZoneConversion, FailedSpawnOutputDoesNotMarkConversionComplete)
    {
        std::filesystem::create_directory(zones_path / "floor-7.toml");
        Zones::ZoneManager::ConvertLegacySpawns("spawns.xml", {});
        EXPECT_FALSE(std::filesystem::exists("spawns.xml.converted"));
        EXPECT_TRUE(std::filesystem::exists("spawns.xml"));
    }

    TEST_F(ZoneConversion, FailedMapCopyDoesNotMarkConversionComplete)
    {
        Zones::ZoneManager::ConvertLegacySpawns("spawns.xml", "missing.otbm");
        EXPECT_FALSE(std::filesystem::exists("spawns.xml.converted"));
        EXPECT_TRUE(std::filesystem::exists("spawns.xml"));
    }

    TEST_F(ZoneConversion, SuccessfulSpawnOutputCreatesMarker)
    {
        Zones::ZoneManager::ConvertLegacySpawns("spawns.xml", {});
        EXPECT_TRUE(std::filesystem::exists("spawns.xml.converted"));
        EXPECT_GT(std::filesystem::file_size(zones_path / "floor-7.toml"), 0u);
    }

    TEST_F(ZoneConversion, FailedFlagOutputDoesNotCreatePatchTarget)
    {
        std::filesystem::create_directory(zones_path / "legacy-flags.toml");
        {
            std::ofstream source("map.otbm", std::ios::binary);
            source << "test";
        }
        Map map;
        map.otbmFilePath = "map.otbm";
        map.legacyZoneFlagTiles.push_back({Position(100, 100, 7), OTBM_TILEFLAG_PROTECTIONZONE, true, {0, 1, 2, 3}});
        Zones::ZoneManager::SmartConvertLegacyZoneFlags(map);
        EXPECT_FALSE(std::filesystem::exists(output_root / "map.otbm"));
    }
}
