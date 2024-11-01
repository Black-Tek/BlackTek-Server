function onUpdateDatabase()
	print(">> Updating database to version 30 (reward boss system)")
	db.query([[
		CREATE TABLE IF NOT EXISTS `player_rewarditems` (
		  `player_id` int NOT NULL,
		  `sid` int NOT NULL COMMENT 'range 0-100 will be reserved for adding items to player who are offline and all > 100 is for items saved from reward chest',
		  `pid` int NOT NULL DEFAULT '0',
		  `itemtype` smallint unsigned NOT NULL,
		  `count` smallint NOT NULL DEFAULT '0',
		  `attributes` blob NOT NULL,
		  UNIQUE KEY `player_id_2` (`player_id`, `sid`),
		  FOREIGN KEY (`player_id`) REFERENCES `players`(`id`) ON DELETE CASCADE
		) ENGINE=InnoDB DEFAULT CHARACTER SET=utf8;
	]])
	return true
end