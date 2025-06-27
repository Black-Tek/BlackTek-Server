function onUpdateDatabase()
	print("> Updating database to version 33 (Custom Skills)")

	-- 1. Create table with correct default blob (4 bytes: uint32_t(0))
	db.query([[
		CREATE TABLE IF NOT EXISTS `player_custom_skills` (
			`player_id` int NOT NULL,
			`skills` blob NOT NULL DEFAULT UNHEX('00000000'),
			FOREIGN KEY (`player_id`) REFERENCES `players`(`id`) ON DELETE CASCADE
		) ENGINE=InnoDB DEFAULT CHARACTER SET=utf8;
	]])

	-- 2. Add columns to existing item tables with correct default
	db.query("ALTER TABLE `player_items` ADD COLUMN `skills` blob NOT NULL DEFAULT UNHEX('00000000')")
	db.query("ALTER TABLE `player_inboxitems` ADD COLUMN `skills` blob NOT NULL DEFAULT UNHEX('00000000')")
	db.query("ALTER TABLE `player_depotitems` ADD COLUMN `skills` blob NOT NULL DEFAULT UNHEX('00000000')")
	db.query("ALTER TABLE `player_rewarditems` ADD COLUMN `skills` blob NOT NULL DEFAULT UNHEX('00000000')")
	db.query("ALTER TABLE `player_storeinboxitems` ADD COLUMN `skills` blob NOT NULL DEFAULT UNHEX('00000000')")

	-- 3. Insert default skills record for existing players (if missing)
	db.query([[
		INSERT INTO `player_custom_skills` (`player_id`, `skills`)
		SELECT `id`, UNHEX('00000000') FROM `players`
		WHERE `id` NOT IN (SELECT `player_id` FROM `player_custom_skills`);
	]])

	-- 4. Ensure all skills blobs are at least a 4-byte count of 0
	db.query("UPDATE `player_items` SET `skills` = UNHEX('00000000') WHERE LENGTH(skills) = 0")
	db.query("UPDATE `player_inboxitems` SET `skills` = UNHEX('00000000') WHERE LENGTH(skills) = 0")
	db.query("UPDATE `player_depotitems` SET `skills` = UNHEX('00000000') WHERE LENGTH(skills) = 0")
	db.query("UPDATE `player_rewarditems` SET `skills` = UNHEX('00000000') WHERE LENGTH(skills) = 0")
	db.query("UPDATE `player_storeinboxitems` SET `skills` = UNHEX('00000000') WHERE LENGTH(skills) = 0")
	db.query("UPDATE `player_custom_skills` SET `skills` = UNHEX('00000000') WHERE LENGTH(skills) = 0")

	return true
end
