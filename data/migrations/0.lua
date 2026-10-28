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
            return db.query(string.format(
                "ALTER TABLE `%s` ADD COLUMN `stats` BLOB NOT NULL AFTER `skills`",
                tableName
            ))
        end
        return true
    end

    -- 1. Create player_custom_stats table
    if not db.query([[
        CREATE TABLE IF NOT EXISTS `player_custom_stats` (
            `player_id` INT NOT NULL,
            `stats` BLOB NOT NULL,
            KEY `player_id` (`player_id`),
            CONSTRAINT `player_custom_stats_ibfk_1`
                FOREIGN KEY (`player_id`)
                REFERENCES `players` (`id`)
                ON DELETE CASCADE
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;
    ]]) then
        print("> Database update failed: could not create player_custom_stats")
        return false
    end

    -- 2. Add `stats` column to item tables
    for _, tableName in ipairs({
        "player_depotitems", "player_inboxitems", "player_items",
        "player_rewarditems", "player_storeinboxitems"
    }) do
        if not addStatsColumn(tableName) then
            print(string.format("> Database update failed: could not add stats to %s", tableName))
            return false
        end
    end

    print("> Database update completed")
    return true
end
