function onUpdateDatabase()
    print("> Updating database to version 1 : Add custom stats support")

    -- Helper: check if column exists
    local function columnExists(tableName, columnName)
        local query = string.format(
            "SELECT 1 FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = '%s' AND COLUMN_NAME = '%s'",
            tableName, columnName
        )
        local queryResult = db.storeQuery(query)
        if queryResult then
            result.free(queryResult)
            return true
        end
        return false
    end

    -- Helper: add stats column if missing
    local function addStatsColumn(tableName)
        if not columnExists(tableName, "stats") then
            print(string.format("  > Adding stats column to %s", tableName))
            db.query(string.format(
                "ALTER TABLE `%s` ADD COLUMN `stats` BLOB NOT NULL AFTER `skills`",
                tableName
            ))
        end
    end

    -- 1. Create player_custom_stats table
    db.query([[
        CREATE TABLE IF NOT EXISTS `player_custom_stats` (
            `player_id` INT NOT NULL,
            `stats` BLOB NOT NULL,
            KEY `player_id` (`player_id`),
            CONSTRAINT `player_custom_stats_ibfk_1`
                FOREIGN KEY (`player_id`)
                REFERENCES `players` (`id`)
                ON DELETE CASCADE
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;
    ]])

    -- 2. Add `stats` column to item tables
    addStatsColumn("player_depotitems")
    addStatsColumn("player_inboxitems")
    addStatsColumn("player_items")
    addStatsColumn("player_rewarditems")
    addStatsColumn("player_storeinboxitems")

    print("> Database update completed")
    return true
end