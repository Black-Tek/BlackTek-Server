-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1:3306
-- Generation Time: Jul 30, 2025 at 03:44 AM
-- Server version: 8.2.0
-- PHP Version: 8.3.0

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";

START TRANSACTION;

SET time_zone = "+00:00";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */
;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */
;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */
;
/*!40101 SET NAMES utf8mb4 */
;	

--
-- Database: `acc_manager`
--

-- --------------------------------------------------------

--
-- Table structure for table `accounts`
--

CREATE TABLE `accounts` (
    `id` int NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `name` varchar(32) NOT NULL,
    `password` char(40) NOT NULL,
    `secret` char(16) DEFAULT NULL,
    `type` int NOT NULL DEFAULT '1',
    `premium_ends_at` int UNSIGNED NOT NULL DEFAULT '0',
    `email` varchar(255) NOT NULL DEFAULT '',
    `creation` int NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `account_bans`
--

CREATE TABLE `account_bans` (
    `account_id` int NOT NULL,
    `reason` varchar(255) NOT NULL,
    `banned_at` bigint NOT NULL,
    `expires_at` bigint NOT NULL,
    `banned_by` int NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `account_ban_history`
--

CREATE TABLE `account_ban_history` (
    `id` int UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `account_id` int NOT NULL,
    `reason` varchar(255) NOT NULL,
    `banned_at` bigint NOT NULL,
    `expired_at` bigint NOT NULL,
    `banned_by` int NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `account_storage`
--

CREATE TABLE `account_storage` (
    `account_id` int NOT NULL,
    `key` int UNSIGNED NOT NULL,
    `value` int NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `account_viplist`
--

CREATE TABLE `account_viplist` (
    `account_id` int NOT NULL COMMENT 'id of account whose viplist entry it is',
    `player_id` int NOT NULL COMMENT 'id of target player of viplist entry',
    `description` varchar(128) NOT NULL DEFAULT '',
    `icon` tinyint UNSIGNED NOT NULL DEFAULT '0',
    `notify` tinyint NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `guilds`
--

CREATE TABLE `guilds` (
    `id` int NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `name` varchar(255) NOT NULL,
    `ownerid` int NOT NULL,
    `creationdata` int NOT NULL,
    `motd` varchar(255) NOT NULL DEFAULT '',
    `balance` bigint UNSIGNED NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

--
-- Triggers `guilds`
--
DELIMITER $$

CREATE TRIGGER `oncreate_guilds` AFTER INSERT ON `guilds` FOR EACH ROW BEGIN
    INSERT INTO `guild_ranks` (`name`, `level`, `guild_id`) VALUES ('the Leader', 3, NEW.`id`);
    INSERT INTO `guild_ranks` (`name`, `level`, `guild_id`) VALUES ('a Vice-Leader', 2, NEW.`id`);
    INSERT INTO `guild_ranks` (`name`, `level`, `guild_id`) VALUES ('a Member', 1, NEW.`id`);
END
$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `guildwar_kills`
--

CREATE TABLE `guildwar_kills` (
    `id` int NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `killer` varchar(50) NOT NULL,
    `target` varchar(50) NOT NULL,
    `killerguild` int NOT NULL DEFAULT '0',
    `targetguild` int NOT NULL DEFAULT '0',
    `warid` int NOT NULL DEFAULT '0',
    `time` bigint NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `guild_invites`
--

CREATE TABLE `guild_invites` (
    `player_id` int NOT NULL DEFAULT '0',
    `guild_id` int NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `guild_membership`
--

CREATE TABLE `guild_membership` (
    `player_id` int NOT NULL,
    `guild_id` int NOT NULL,
    `rank_id` int NOT NULL,
    `nick` varchar(15) NOT NULL DEFAULT ''
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `guild_ranks`
--

CREATE TABLE `guild_ranks` (
    `id` int NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `guild_id` int NOT NULL COMMENT 'guild',
    `name` varchar(255) NOT NULL COMMENT 'rank name',
    `level` int NOT NULL COMMENT 'rank level - leader, vice, member, maybe something else'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `guild_wars`
--

CREATE TABLE `guild_wars` (
    `id` int NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `guild1` int NOT NULL DEFAULT '0',
    `guild2` int NOT NULL DEFAULT '0',
    `name1` varchar(255) NOT NULL,
    `name2` varchar(255) NOT NULL,
    `status` tinyint NOT NULL DEFAULT '0',
    `started` bigint NOT NULL DEFAULT '0',
    `ended` bigint NOT NULL DEFAULT '0',
    `frags_to_end` int NOT NULL DEFAULT '30'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `houses`
--

CREATE TABLE `houses` (
    `id` int NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `owner` int NOT NULL,
    `paid` int UNSIGNED NOT NULL DEFAULT '0',
    `warnings` int NOT NULL DEFAULT '0',
    `name` varchar(255) NOT NULL,
    `rent` int NOT NULL DEFAULT '0',
    `town_id` int NOT NULL DEFAULT '0',
    `bid` int NOT NULL DEFAULT '0',
    `bid_end` int NOT NULL DEFAULT '0',
    `last_bid` int NOT NULL DEFAULT '0',
    `highest_bidder` int NOT NULL DEFAULT '0',
    `size` int NOT NULL DEFAULT '0',
    `beds` int NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `house_lists`
--

CREATE TABLE `house_lists` (
    `house_id` int NOT NULL,
    `listid` int NOT NULL,
    `list` text NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `ip_bans`
--

CREATE TABLE `ip_bans` (
    `ip` int UNSIGNED NOT NULL,
    `reason` varchar(255) NOT NULL,
    `banned_at` bigint NOT NULL,
    `expires_at` bigint NOT NULL,
    `banned_by` int NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `market_history`
--

CREATE TABLE `market_history` (
    `id` int UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `player_id` int NOT NULL,
    `sale` tinyint NOT NULL DEFAULT '0',
    `itemtype` smallint UNSIGNED NOT NULL,
    `amount` smallint UNSIGNED NOT NULL,
    `price` int UNSIGNED NOT NULL DEFAULT '0',
    `expires_at` bigint UNSIGNED NOT NULL,
    `inserted` bigint UNSIGNED NOT NULL,
    `state` tinyint UNSIGNED NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `market_offers`
--

CREATE TABLE `market_offers` (
    `id` int UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `player_id` int NOT NULL,
    `sale` tinyint NOT NULL DEFAULT '0',
    `itemtype` smallint UNSIGNED NOT NULL,
    `amount` smallint UNSIGNED NOT NULL,
    `created` bigint UNSIGNED NOT NULL,
    `anonymous` tinyint NOT NULL DEFAULT '0',
    `price` int UNSIGNED NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `players`
--

CREATE TABLE `players` (
    `id` int NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `name` varchar(255) NOT NULL,
    `group_id` int NOT NULL DEFAULT '1',
    `account_id` int NOT NULL DEFAULT '0',
    `level` int NOT NULL DEFAULT '1',
    `vocation` int NOT NULL DEFAULT '0',
    `health` int NOT NULL DEFAULT '150',
    `healthmax` int NOT NULL DEFAULT '150',
    `experience` bigint UNSIGNED NOT NULL DEFAULT '0',
    `lookbody` int NOT NULL DEFAULT '0',
    `lookfeet` int NOT NULL DEFAULT '0',
    `lookhead` int NOT NULL DEFAULT '0',
    `looklegs` int NOT NULL DEFAULT '0',
    `looktype` int NOT NULL DEFAULT '136',
    `lookaddons` int NOT NULL DEFAULT '0',
    `direction` tinyint UNSIGNED NOT NULL DEFAULT '2',
    `maglevel` int NOT NULL DEFAULT '0',
    `mana` int NOT NULL DEFAULT '0',
    `manamax` int NOT NULL DEFAULT '0',
    `manaspent` bigint UNSIGNED NOT NULL DEFAULT '0',
    `soul` int UNSIGNED NOT NULL DEFAULT '0',
    `town_id` int NOT NULL DEFAULT '1',
    `posx` int NOT NULL DEFAULT '0',
    `posy` int NOT NULL DEFAULT '0',
    `posz` int NOT NULL DEFAULT '0',
    `conditions` blob,
    `cap` int NOT NULL DEFAULT '400',
    `sex` int NOT NULL DEFAULT '0',
    `lastlogin` bigint UNSIGNED NOT NULL DEFAULT '0',
    `lastip` int UNSIGNED NOT NULL DEFAULT '0',
    `save` tinyint NOT NULL DEFAULT '1',
    `skull` tinyint NOT NULL DEFAULT '0',
    `skulltime` bigint NOT NULL DEFAULT '0',
    `lastlogout` bigint UNSIGNED NOT NULL DEFAULT '0',
    `blessings` tinyint NOT NULL DEFAULT '0',
    `onlinetime` bigint NOT NULL DEFAULT '0',
    `deletion` bigint NOT NULL DEFAULT '0',
    `balance` bigint UNSIGNED NOT NULL DEFAULT '0',
    `offlinetraining_time` smallint UNSIGNED NOT NULL DEFAULT '43200',
    `offlinetraining_skill` int NOT NULL DEFAULT '-1',
    `stamina` smallint UNSIGNED NOT NULL DEFAULT '2520',
    `skill_fist` int UNSIGNED NOT NULL DEFAULT '10',
    `skill_fist_tries` bigint UNSIGNED NOT NULL DEFAULT '0',
    `skill_club` int UNSIGNED NOT NULL DEFAULT '10',
    `skill_club_tries` bigint UNSIGNED NOT NULL DEFAULT '0',
    `skill_sword` int UNSIGNED NOT NULL DEFAULT '10',
    `skill_sword_tries` bigint UNSIGNED NOT NULL DEFAULT '0',
    `skill_axe` int UNSIGNED NOT NULL DEFAULT '10',
    `skill_axe_tries` bigint UNSIGNED NOT NULL DEFAULT '0',
    `skill_dist` int UNSIGNED NOT NULL DEFAULT '10',
    `skill_dist_tries` bigint UNSIGNED NOT NULL DEFAULT '0',
    `skill_shielding` int UNSIGNED NOT NULL DEFAULT '10',
    `skill_shielding_tries` bigint UNSIGNED NOT NULL DEFAULT '0',
    `skill_fishing` int UNSIGNED NOT NULL DEFAULT '10',
    `skill_fishing_tries` bigint UNSIGNED NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Triggers `players`
--
DELIMITER $$

CREATE TRIGGER `ondelete_players` BEFORE DELETE ON `players` FOR EACH ROW BEGIN
    UPDATE `houses` SET `owner` = 0 WHERE `owner` = OLD.`id`;
END
$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `players_online`
--

CREATE TABLE `players_online` (`player_id` int NOT NULL) ENGINE = MEMORY DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_augments`
--

CREATE TABLE `player_augments` (
    `player_id` int NOT NULL,
    `augments` blob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_custom_skills`
--

CREATE TABLE `player_custom_skills` (
    `player_id` int NOT NULL,
    `skills` blob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_custom_stats`
--

CREATE TABLE `player_custom_stats` (
    `player_id` int NOT NULL,
    `stats` blob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_deaths`
--

CREATE TABLE `player_deaths` (
    `player_id` int NOT NULL,
    `time` bigint UNSIGNED NOT NULL DEFAULT '0',
    `level` int NOT NULL DEFAULT '1',
    `killed_by` varchar(255) NOT NULL,
    `is_player` tinyint NOT NULL DEFAULT '1',
    `mostdamage_by` varchar(100) NOT NULL,
    `mostdamage_is_player` tinyint NOT NULL DEFAULT '0',
    `unjustified` tinyint NOT NULL DEFAULT '0',
    `mostdamage_unjustified` tinyint NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_depotitems`
--

CREATE TABLE `player_depotitems` (
    `player_id` int NOT NULL,
    `sid` int NOT NULL COMMENT 'any given range eg 0-100 will be reserved for depot lockers and all > 100 will be then normal items inside depots',
    `pid` int NOT NULL DEFAULT '0',
    `itemtype` smallint UNSIGNED NOT NULL,
    `count` smallint NOT NULL DEFAULT '0',
    `attributes` blob NOT NULL,
    `augments` blob NOT NULL,
    `skills` blob NOT NULL,
    `stats` blob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_inboxitems`
--

CREATE TABLE `player_inboxitems` (
    `player_id` int NOT NULL,
    `sid` int NOT NULL,
    `pid` int NOT NULL DEFAULT '0',
    `itemtype` smallint UNSIGNED NOT NULL,
    `count` smallint NOT NULL DEFAULT '0',
    `attributes` blob NOT NULL,
    `augments` blob NOT NULL,
    `skills` blob NOT NULL,
    `stats` blob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_items`
--

CREATE TABLE `player_items` (
    `player_id` int NOT NULL DEFAULT '0',
    `pid` int NOT NULL DEFAULT '0',
    `sid` int NOT NULL DEFAULT '0',
    `itemtype` smallint UNSIGNED NOT NULL DEFAULT '0',
    `count` smallint NOT NULL DEFAULT '0',
    `attributes` blob NOT NULL,
    `augments` blob NOT NULL,
    `skills` blob NOT NULL,
    `stats` blob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_namelocks`
--

CREATE TABLE `player_namelocks` (
    `player_id` int NOT NULL,
    `reason` varchar(255) NOT NULL,
    `namelocked_at` bigint NOT NULL,
    `namelocked_by` int NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_rewarditems`
--

CREATE TABLE `player_rewarditems` (
    `player_id` int NOT NULL,
    `sid` int NOT NULL COMMENT 'range 0-100 will be reserved for adding items to player who are offline and all > 100 is for items saved from reward chest',
    `pid` int NOT NULL DEFAULT '0',
    `itemtype` smallint UNSIGNED NOT NULL,
    `count` smallint NOT NULL DEFAULT '0',
    `attributes` blob NOT NULL,
    `augments` blob NOT NULL,
    `skills` blob NOT NULL,
    `stats` blob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_spells`
--

CREATE TABLE `player_spells` (
    `player_id` int NOT NULL,
    `name` varchar(255) NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_storage`
--

CREATE TABLE `player_storage` (
    `player_id` int NOT NULL DEFAULT '0',
    `key` int UNSIGNED NOT NULL DEFAULT '0',
    `value` int NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `player_storeinboxitems`
--

CREATE TABLE `player_storeinboxitems` (
    `player_id` int NOT NULL,
    `sid` int NOT NULL,
    `pid` int NOT NULL DEFAULT '0',
    `itemtype` smallint UNSIGNED NOT NULL,
    `count` smallint NOT NULL DEFAULT '0',
    `attributes` blob NOT NULL,
    `augments` blob NOT NULL,
    `skills` blob NOT NULL,
    `stats` blob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `server_config`
--

CREATE TABLE `server_config` (
    `config` varchar(50) NOT NULL,
    `value` varchar(256) NOT NULL DEFAULT ''
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

INSERT INTO
    `server_config` (`config`, `value`)
VALUES ('db_version', '0');

-- --------------------------------------------------------

--
-- Table structure for table `tile_store`
--

CREATE TABLE `tile_store` (
    `house_id` int NOT NULL,
    `data` longblob NOT NULL
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Table structure for table `towns`
--

CREATE TABLE `towns` (
    `id` int NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `name` varchar(255) NOT NULL,
    `posx` int NOT NULL DEFAULT '0',
    `posy` int NOT NULL DEFAULT '0',
    `posz` int NOT NULL DEFAULT '0'
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb3;

-- --------------------------------------------------------

--
-- Indexes for table `accounts`
--
ALTER TABLE `accounts` ADD UNIQUE KEY `name` (`name`);

--
-- Indexes for table `account_bans`
--
ALTER TABLE `account_bans`
ADD PRIMARY KEY (`account_id`),
ADD KEY `banned_by` (`banned_by`);

--
-- Indexes for table `account_ban_history`
--
ALTER TABLE `account_ban_history`
ADD KEY `account_id` (`account_id`),
ADD KEY `banned_by` (`banned_by`);

--
-- Indexes for table `account_storage`
--
ALTER TABLE `account_storage` ADD PRIMARY KEY (`account_id`, `key`);

--
-- Indexes for table `account_viplist`
--
ALTER TABLE `account_viplist`
ADD UNIQUE KEY `account_player_index` (`account_id`, `player_id`),
ADD KEY `player_id` (`player_id`);

--
-- Indexes for table `guilds`
--
ALTER TABLE `guilds`
ADD UNIQUE KEY `name` (`name`),
ADD UNIQUE KEY `ownerid` (`ownerid`);

--
-- Indexes for table `guildwar_kills`
--
ALTER TABLE `guildwar_kills` ADD KEY `warid` (`warid`);

--
-- Indexes for table `guild_invites`
--
ALTER TABLE `guild_invites`
ADD PRIMARY KEY (`player_id`, `guild_id`),
ADD KEY `guild_id` (`guild_id`);

--
-- Indexes for table `guild_membership`
--
ALTER TABLE `guild_membership`
ADD PRIMARY KEY (`player_id`),
ADD KEY `guild_id` (`guild_id`),
ADD KEY `rank_id` (`rank_id`);

--
-- Indexes for table `guild_ranks`
--
ALTER TABLE `guild_ranks` ADD KEY `guild_id` (`guild_id`);

--
-- Indexes for table `guild_wars`
--
ALTER TABLE `guild_wars`
ADD KEY `guild1` (`guild1`),
ADD KEY `guild2` (`guild2`);

--
-- Indexes for table `houses`
--
ALTER TABLE `houses`
ADD KEY `owner` (`owner`),
ADD KEY `town_id` (`town_id`);

--
-- Indexes for table `house_lists`
--
ALTER TABLE `house_lists` ADD KEY `house_id` (`house_id`);

--
-- Indexes for table `ip_bans`
--
ALTER TABLE `ip_bans`
ADD PRIMARY KEY (`ip`),
ADD KEY `banned_by` (`banned_by`);

--
-- Indexes for table `market_history`
--
ALTER TABLE `market_history`
ADD KEY `player_id` (`player_id`, `sale`);

--
-- Indexes for table `market_offers`
--
ALTER TABLE `market_offers`
ADD KEY `sale` (`sale`, `itemtype`),
ADD KEY `created` (`created`),
ADD KEY `player_id` (`player_id`);

--
-- Indexes for table `players`
--
ALTER TABLE `players`
ADD UNIQUE KEY `name` (`name`),
ADD KEY `account_id` (`account_id`),
ADD KEY `vocation` (`vocation`);

--
-- Indexes for table `players_online`
--
ALTER TABLE `players_online` ADD PRIMARY KEY (`player_id`);

--
-- Indexes for table `player_augments`
--
ALTER TABLE `player_augments` ADD KEY `player_id` (`player_id`);

--
-- Indexes for table `player_custom_skills`
--
ALTER TABLE `player_custom_skills`
ADD KEY `player_id` (`player_id`);

--
-- Indexes for table `player_custom_stats`
--
ALTER TABLE `player_custom_stats`
ADD KEY `player_id` (`player_id`);

--
-- Indexes for table `player_deaths`
--
ALTER TABLE `player_deaths`
ADD KEY `player_id` (`player_id`),
ADD KEY `killed_by` (`killed_by`),
ADD KEY `mostdamage_by` (`mostdamage_by`);

--
-- Indexes for table `player_depotitems`
--
ALTER TABLE `player_depotitems`
ADD UNIQUE KEY `player_id_2` (`player_id`, `sid`);

--
-- Indexes for table `player_inboxitems`
--
ALTER TABLE `player_inboxitems`
ADD UNIQUE KEY `player_id_2` (`player_id`, `sid`);

--
-- Indexes for table `player_items`
--
ALTER TABLE `player_items`
ADD KEY `player_id` (`player_id`),
ADD KEY `sid` (`sid`);

--
-- Indexes for table `player_namelocks`
--
ALTER TABLE `player_namelocks`
ADD PRIMARY KEY (`player_id`),
ADD KEY `namelocked_by` (`namelocked_by`);

--
-- Indexes for table `player_rewarditems`
--
ALTER TABLE `player_rewarditems`
ADD UNIQUE KEY `player_id_2` (`player_id`, `sid`);

--
-- Indexes for table `player_spells`
--
ALTER TABLE `player_spells` ADD KEY `player_id` (`player_id`);

--
-- Indexes for table `player_storage`
--
ALTER TABLE `player_storage` ADD PRIMARY KEY (`player_id`, `key`);

--
-- Indexes for table `player_storeinboxitems`
--
ALTER TABLE `player_storeinboxitems`
ADD UNIQUE KEY `player_id_2` (`player_id`, `sid`);

--
-- Indexes for table `server_config`
--
ALTER TABLE `server_config` ADD PRIMARY KEY (`config`);

--
-- Indexes for table `tile_store`
--
ALTER TABLE `tile_store` ADD KEY `house_id` (`house_id`);

--
-- Indexes for table `towns`
--
ALTER TABLE `towns` ADD UNIQUE KEY `name` (`name`);

--
-- Constraints for dumped tables
--

--
-- Account Manager
--
INSERT INTO
    `accounts` (
        `name`,
        `password`,
        `secret`,
        `type`,
        `premium_ends_at`,
        `email`,
        `creation`
    )
VALUES (
        '1',
        '356a192b7913b04c54574d18c28d46e6395428ab',
        NULL,
        1,
        0,
        '',
        0
    );

INSERT INTO
    `players` (
        `name`,
        `group_id`,
        `account_id`,
        `level`,
        `vocation`,
        `health`,
        `healthmax`,
        `experience`,
        `lookbody`,
        `lookfeet`,
        `lookhead`,
        `looklegs`,
        `looktype`,
        `lookaddons`,
        `direction`,
        `maglevel`,
        `mana`,
        `manamax`,
        `manaspent`,
        `soul`,
        `town_id`,
        `posx`,
        `posy`,
        `posz`,
        `conditions`,
        `cap`,
        `sex`,
        `lastlogin`,
        `lastip`,
        `save`,
        `skull`,
        `skulltime`,
        `lastlogout`,
        `blessings`,
        `onlinetime`,
        `deletion`,
        `balance`,
        `stamina`,
        `skill_fist`,
        `skill_fist_tries`,
        `skill_club`,
        `skill_club_tries`,
        `skill_sword`,
        `skill_sword_tries`,
        `skill_axe`,
        `skill_axe_tries`,
        `skill_dist`,
        `skill_dist_tries`,
        `skill_shielding`,
        `skill_shielding_tries`,
        `skill_fishing`,
        `skill_fishing_tries`
    )
VALUES (
        'Account Manager',
        1,
        1,
        1,
        0,
        150,
        150,
        0,
        0,
        0,
        0,
        0,
        110,
        0,
        2,
        0,
        0,
        0,
        0,
        0,
        1,
        2500,
        2500,
        7,
        '',
        400,
        1,
        1703277227,
        16777343,
        1,
        0,
        0,
        1710912621,
        0,
        7641057,
        0,
        0,
        2520,
        10,
        0,
        10,
        0,
        10,
        0,
        10,
        0,
        10,
        0,
        10,
        0,
        10,
        0
    );

--
-- Constraints for table `account_bans`
--
ALTER TABLE `account_bans`
ADD CONSTRAINT `account_bans_ibfk_1` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
ADD CONSTRAINT `account_bans_ibfk_2` FOREIGN KEY (`banned_by`) REFERENCES `players` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `account_ban_history`
--
ALTER TABLE `account_ban_history`
ADD CONSTRAINT `account_ban_history_ibfk_1` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
ADD CONSTRAINT `account_ban_history_ibfk_2` FOREIGN KEY (`banned_by`) REFERENCES `players` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `account_storage`
--
ALTER TABLE `account_storage`
ADD CONSTRAINT `account_storage_ibfk_1` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `account_viplist`
--
ALTER TABLE `account_viplist`
ADD CONSTRAINT `account_viplist_ibfk_1` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE,
ADD CONSTRAINT `account_viplist_ibfk_2` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `guilds`
--
ALTER TABLE `guilds`
ADD CONSTRAINT `guilds_ibfk_1` FOREIGN KEY (`ownerid`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `guildwar_kills`
--
ALTER TABLE `guildwar_kills`
ADD CONSTRAINT `guildwar_kills_ibfk_1` FOREIGN KEY (`warid`) REFERENCES `guild_wars` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `guild_invites`
--
ALTER TABLE `guild_invites`
ADD CONSTRAINT `guild_invites_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE,
ADD CONSTRAINT `guild_invites_ibfk_2` FOREIGN KEY (`guild_id`) REFERENCES `guilds` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `guild_membership`
--
ALTER TABLE `guild_membership`
ADD CONSTRAINT `guild_membership_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
ADD CONSTRAINT `guild_membership_ibfk_2` FOREIGN KEY (`guild_id`) REFERENCES `guilds` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
ADD CONSTRAINT `guild_membership_ibfk_3` FOREIGN KEY (`rank_id`) REFERENCES `guild_ranks` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `guild_ranks`
--
ALTER TABLE `guild_ranks`
ADD CONSTRAINT `guild_ranks_ibfk_1` FOREIGN KEY (`guild_id`) REFERENCES `guilds` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `house_lists`
--
ALTER TABLE `house_lists`
ADD CONSTRAINT `house_lists_ibfk_1` FOREIGN KEY (`house_id`) REFERENCES `houses` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `ip_bans`
--
ALTER TABLE `ip_bans`
ADD CONSTRAINT `ip_bans_ibfk_1` FOREIGN KEY (`banned_by`) REFERENCES `players` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `market_history`
--
ALTER TABLE `market_history`
ADD CONSTRAINT `market_history_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `market_offers`
--
ALTER TABLE `market_offers`
ADD CONSTRAINT `market_offers_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `players`
--
ALTER TABLE `players`
ADD CONSTRAINT `players_ibfk_1` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_augments`
--
ALTER TABLE `player_augments`
ADD CONSTRAINT `player_augments_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_custom_skills`
--
ALTER TABLE `player_custom_skills`
ADD CONSTRAINT `player_custom_skills_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_custom_stats`
--
ALTER TABLE `player_custom_stats`
ADD CONSTRAINT `player_custom_stats_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_deaths`
--
ALTER TABLE `player_deaths`
ADD CONSTRAINT `player_deaths_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_depotitems`
--
ALTER TABLE `player_depotitems`
ADD CONSTRAINT `player_depotitems_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_inboxitems`
--
ALTER TABLE `player_inboxitems`
ADD CONSTRAINT `player_inboxitems_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_items`
--
ALTER TABLE `player_items`
ADD CONSTRAINT `player_items_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_namelocks`
--
ALTER TABLE `player_namelocks`
ADD CONSTRAINT `player_namelocks_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
ADD CONSTRAINT `player_namelocks_ibfk_2` FOREIGN KEY (`namelocked_by`) REFERENCES `players` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `player_rewarditems`
--
ALTER TABLE `player_rewarditems`
ADD CONSTRAINT `player_rewarditems_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_spells`
--
ALTER TABLE `player_spells`
ADD CONSTRAINT `player_spells_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_storage`
--
ALTER TABLE `player_storage`
ADD CONSTRAINT `player_storage_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `player_storeinboxitems`
--
ALTER TABLE `player_storeinboxitems`
ADD CONSTRAINT `player_storeinboxitems_ibfk_1` FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE;

--
-- Constraints for table `tile_store`
--
ALTER TABLE `tile_store`
ADD CONSTRAINT `tile_store_ibfk_1` FOREIGN KEY (`house_id`) REFERENCES `houses` (`id`) ON DELETE CASCADE;

COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */
;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */
;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */
;
