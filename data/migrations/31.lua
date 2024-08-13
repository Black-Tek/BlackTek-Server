function onUpdateDatabase()
	print("> Updating database to version 31 (Augments)")
	db.query([[
		CREATE TABLE IF NOT EXISTS `player_augments` (
		  `player_id` int NOT NULL,
		  `augments` blob NOT NULL,
		  FOREIGN KEY (`player_id`) REFERENCES `players`(`id`) ON DELETE CASCADE
		) ENGINE=InnoDB DEFAULT CHARACTER SET=utf8;
	]])

	db.query("ALTER TABLE `player_items` ADD `augments` blob NOT NULL")
	db.query("ALTER TABLE `player_inboxitems` ADD `augments` blob NOT NULL")
	db.query("ALTER TABLE `player_depotitems` ADD `augments` blob NOT NULL")
	db.query("ALTER TABLE `player_rewarditems` ADD `augments` blob NOT NULL")
	db.query("ALTER TABLE `player_storeinboxitems` ADD `augments` blob NOT NULL")
	return true
end