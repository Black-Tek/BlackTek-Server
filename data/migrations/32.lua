function onUpdateDatabase()
	print("> Updating database to version 32 (Initial Guilds Rework)")

	db.query("ALTER TABLE `guild_wars` ADD COLUMN `frags_to_end` INT NOT NULL DEFAULT 30");
	db.query("ALTER TABLE `guilds` ADD COLUMN `balance` BIGINT UNSIGNED NOT NULL DEFAULT 0");

	return true
end