// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "pugicast.h"

#include "actions.h"
#include "bed.h"
#include "configmanager.h"
#include "creature.h"
#include "creatureevent.h"
#include "databasetasks.h"
#include "events.h"
#include "game.h"
#include "globalevent.h"
#include "iologindata.h"
#include "iomarket.h"
#include "items.h"
#include "monster.h"
#include "movement.h"
#include "scheduler.h"
#include "server.h"
#include "spells.h"
#include "talkaction.h"
#include "weapons.h"
#include "script.h"

#include <fmt/format.h>

extern ConfigManager g_config;
extern Actions* g_actions;
extern Augments* g_augments;
extern Chat* g_chat;
extern TalkActions* g_talkActions;
extern Spells* g_spells;
extern Vocations g_vocations;
extern GlobalEvents* g_globalEvents;
extern CreatureEvents* g_creatureEvents;
extern Events* g_events;
extern Monsters g_monsters;
extern MoveEvents* g_moveEvents;
extern Weapons* g_weapons;
extern Scripts* g_scripts;

Game::Game()
{
	offlineTrainingWindow.defaultEnterButton = 0;
	offlineTrainingWindow.defaultEscapeButton = 1;
	offlineTrainingWindow.choices.emplace_back("Sword Fighting and Shielding", SKILL_SWORD);
	offlineTrainingWindow.choices.emplace_back("Axe Fighting and Shielding", SKILL_AXE);
	offlineTrainingWindow.choices.emplace_back("Club Fighting and Shielding", SKILL_CLUB);
	offlineTrainingWindow.choices.emplace_back("Distance Fighting and Shielding", SKILL_DISTANCE);
	offlineTrainingWindow.choices.emplace_back("Magic Level and Shielding", SKILL_MAGLEVEL);
	offlineTrainingWindow.buttons.emplace_back("Okay", offlineTrainingWindow.defaultEnterButton);
	offlineTrainingWindow.buttons.emplace_back("Cancel", offlineTrainingWindow.defaultEscapeButton);
	offlineTrainingWindow.priority = true;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
}

Game::~Game()
{
	curl_global_cleanup();
	curl_easy_cleanup(curl);

	for (const auto& it : guilds) {
		delete it.second;
	}
}

void Game::start(ServiceManager* manager)
{
	serviceManager = manager;
	updateWorldTime();

	if (g_config.getBoolean(ConfigManager::DEFAULT_WORLD_LIGHT)) {
		g_scheduler.addEvent(createSchedulerTask(EVENT_LIGHTINTERVAL, [this]() { checkLight(); }));
	}
	g_scheduler.addEvent(createSchedulerTask(EVENT_CREATURE_THINK_INTERVAL, [this]() { checkCreatures(0); }));
	g_scheduler.addEvent(createSchedulerTask(EVENT_DECAYINTERVAL, [this]() { checkDecay(); }));
}

GameState_t Game::getGameState() const
{
	return gameState;
}

void Game::setWorldType(WorldType_t type)
{
	worldType = type;
}

void Game::setGameState(GameState_t newState)
{
	if (gameState == GAME_STATE_SHUTDOWN) {
		return; //this cannot be stopped
	}

	if (gameState == newState) {
		return;
	}

	gameState = newState;
	switch (newState) {
		case GAME_STATE_INIT: {
			groups.load();
			g_chat->load();

			map.spawns.startup();

			raids.loadFromXml();
			raids.startup();

			quests.loadFromXml();
			mounts.loadFromToml();

			loadMotdNum();
			loadPlayersRecord();
			loadAccountStorageValues();

			g_globalEvents->startup();
			break;
		}

		case GAME_STATE_SHUTDOWN: {
			g_globalEvents->save();
			g_globalEvents->shutdown();

			//kick all players that are still online
			auto it = players.begin();
			while (it != players.end()) {
				it->second->kickPlayer(true);
				it = players.begin();
			}

			saveMotdNum();
			saveGameState();

			g_dispatcher.addTask(
				createTask([this]() { shutdown(); }));

			g_scheduler.stop();
			g_databaseTasks.stop();
			g_dispatcher.stop();
			g_dispatcher_discord.stop();
			break;
		}

		case GAME_STATE_CLOSED: {
			g_globalEvents->save();
			/* kick all players without the CanAlwaysLogin flag */
			auto it = players.begin();
			while (it != players.end()) {
				if (!it->second->hasFlag(PlayerFlag_CanAlwaysLogin)) {
					it->second->kickPlayer(true);
					it = players.begin();
				} else {
					++it;
				}
			}

			saveGameState();
			break;
		}

		default:
			break;
	}
}

void Game::saveGameState()
{
	if (gameState == GAME_STATE_NORMAL) {
		setGameState(GAME_STATE_MAINTAIN);
	}

	std::cout << "Saving server..." << std::endl;

	if (!saveAccountStorageValues()) {
		std::cout << "[Error - Game::saveGameState] Failed to save account-level storage values." << std::endl;
	}

	for (const auto& it : players) {
		it.second->loginPosition = it.second->getPosition();
		IOLoginData::savePlayer(it.second);
	}

	Map::save();

	g_databaseTasks.flush();

	if (gameState == GAME_STATE_MAINTAIN) {
		setGameState(GAME_STATE_NORMAL);
	}
}

bool Game::loadMainMap(const std::string& filename)
{
	if (map.loadMap("data/world/" + filename + ".otbm", true)) {
		for (auto& [id, house] : g_game.map.houses.getHouses()) {
			for (auto& tile : house->getTiles()) {
				if (auto itemlist = tile->getItemList()) {
					for (auto& item : *itemlist) {
						if (item->getDoor() && !house->getDoorByPosition(item->getPosition())) {
							if (item->getDoor()->getDoorId() != 0) {
								house->addDoor(item->getDoor());
							}
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

void Game::loadMap(const std::string& path)
{
	map.loadMap(path, false);
}

CylinderPtr Game::internalGetCylinder(const PlayerPtr& player, const Position& pos)
{
	if (pos.x != 0xFFFF) {
		return map.getTile(pos);
	}

	//container
	if (pos.y & 0x40) {
		uint8_t from_cid = pos.y & 0x0F;
		return player->getContainerByID(from_cid);
	}

	//inventory
	return player;
}

ThingPtr Game::internalGetThing(const PlayerPtr& player, const Position& pos, int32_t index, uint32_t spriteId, stackPosType_t type)
{
	if (pos.x != 0xFFFF) {
		auto tile = map.getTile(pos);
		if (!tile) {
			return nullptr;
		}

		ThingPtr thing;
		switch (type) {
			case STACKPOS_LOOK: {
				return tile->getTopVisibleThing(player);
			}

			case STACKPOS_MOVE: {
				auto item = tile->getTopDownItem();
				if (item && item->isMoveable()) {
					thing = item;
				} else {
					thing = tile->getTopVisibleCreature(player);
				}
				break;
			}

			case STACKPOS_USEITEM: {
				thing = tile->getUseItem(index);
				break;
			}

			case STACKPOS_TOPDOWN_ITEM: {
				thing = tile->getTopDownItem();
				break;
			}

			case STACKPOS_USETARGET: {
				thing = tile->getTopVisibleCreature(player);
				if (!thing) {
					thing = tile->getUseItem(index);
				}
				break;
			}

			default: {
				thing = nullptr;
				break;
			}
		}
		if (player && tile->hasFlag(TILESTATE_SUPPORTS_HANGABLE)) {
			// do extra checks here if the thing is accessible
			if (thing && thing->getItem()) {
				if (tile->hasProperty(CONST_PROP_ISVERTICAL)) {
					if (player->getPosition().x + 1 == tile->getPosition().x) {
						thing = nullptr;
					}
				}
				else { // horizontal
					if (player->getPosition().y + 1 == tile->getPosition().y) {
						thing = nullptr;
					}
				}
			}
		}
		return thing;
	}

	//container
	if (pos.y & 0x40) {
		uint8_t fromCid = pos.y & 0x0F;

		auto parentContainer = player->getContainerByID(fromCid);
		if (!parentContainer) {
			return nullptr;
		}

		if (parentContainer->getID() == ITEM_BROWSEFIELD) {
			auto tile = parentContainer->getTile();
			if (tile && tile->hasFlag(TILESTATE_SUPPORTS_HANGABLE)) {
				if (tile->hasProperty(CONST_PROP_ISVERTICAL)) {
					if (player->getPosition().x + 1 == tile->getPosition().x) {
						return nullptr;
					}
				} else { // horizontal
					if (player->getPosition().y + 1 == tile->getPosition().y) {
						return nullptr;
					}
				}
			}
		}

		uint8_t slot = pos.z;
		return parentContainer->getItemByIndex(player->getContainerIndex(fromCid) + slot);
	} else if (pos.y == 0 && pos.z == 0) {
		const ItemType& it = Item::items.getItemIdByClientId(spriteId);
		if (it.id == 0) {
			return nullptr;
		}

		int32_t subType;
		if (it.isFluidContainer() && index < static_cast<int32_t>(sizeof(reverseFluidMap) / sizeof(uint8_t))) {
			subType = reverseFluidMap[index];
		} else {
			subType = -1;
		}

		return findItemOfType(player, it.id, true, subType);
	}

	//inventory
	slots_t slot = static_cast<slots_t>(pos.y);
	if (slot == CONST_SLOT_STORE_INBOX) {
		return player->getStoreInbox();
	}

	return player->getInventoryItem(slot);
}

void Game::internalGetPosition(const ItemPtr& item, Position& pos, uint8_t& stackpos)
{
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	stackpos = 0;

	
	if (auto topParent = item->getTopParent()) {
		if (auto player = std::dynamic_pointer_cast<Player>(topParent)) {
			pos.x = 0xFFFF;

                if (const auto container = std::dynamic_pointer_cast<Container>(item->getParent())) {
                    pos.y = static_cast<uint16_t>(0x40) | static_cast<uint16_t>(player->getContainerID(std::dynamic_pointer_cast<const Container>(container)));
                    pos.z = container->getThingIndex(item);
                    stackpos = pos.z;
                } else {
				pos.y = player->getThingIndex(item);
				stackpos = pos.y;
			}
		} else if (auto tile = topParent->getTile()) {
			pos = tile->getPosition();
			stackpos = tile->getThingIndex(item);
		}
	}
}

CreaturePtr Game::getCreatureByID(const uint32_t id)
{
	if (id <= Player::playerAutoID) {
		return getPlayerByID(id);
	} else if (id <= Monster::monsterAutoID) {
		return getMonsterByID(id);
	} else if (id <= Npc::npcAutoID) {
		return getNpcByID(id);
	}
	return nullptr;
}

MonsterPtr Game::getMonsterByID(const uint32_t id)
{
	if (id == 0) {
		return nullptr;
	}

	const auto it = monsters.find(id);
	if (it == monsters.end()) {
		return nullptr;
	}
	return it->second;
}

NpcPtr Game::getNpcByID(const uint32_t id)
{
	if (id == 0) {
		return nullptr;
	}

	const auto it = npcs.find(id);
	if (it == npcs.end()) {
		return nullptr;
	}
	return it->second;
}

PlayerPtr Game::getPlayerByID(const uint32_t id)
{
	if (id == 0) {
		return nullptr;
	}

	const auto it = players.find(id);
	if (it == players.end()) {
		return nullptr;
	}
	return it->second;
}

CreaturePtr Game::getCreatureByName(const std::string& s)
{
	if (s.empty()) {
		return nullptr;
	}

	const std::string& lowerCaseName = asLowerCaseString(s);

	{
		const auto it = mappedPlayerNames.find(lowerCaseName);
		if (it != mappedPlayerNames.end()) {
			return it->second;
		}
	}

	auto equalCreatureName = [&](const std::pair<uint32_t, CreaturePtr>& it) {
		auto name = it.second->getName();
		return lowerCaseName.size() == name.size() && std::equal(lowerCaseName.begin(), lowerCaseName.end(), name.begin(), [](char a, char b) {
			return a == std::tolower(b);
		});
	};

	{
		if (const auto it = std::ranges::find_if(npcs, equalCreatureName); it != npcs.end()) {
			return it->second;
		}
	}

	{
		if (const auto it = std::ranges::find_if(monsters, equalCreatureName); it != monsters.end()) {
			return it->second;
		}
	}

	return nullptr;
}

NpcPtr Game::getNpcByName(const std::string& s) const
{
	if (s.empty()) {
		return nullptr;
	}

	const char* npcName = s.c_str();
	for (const auto& val : npcs | std::views::values) {
		if (caseInsensitiveEqual(npcName, val->getName())) {
			return val;
		}
	}
	return nullptr;
}

PlayerPtr Game::getPlayerByName(const std::string& s)
{
	if (s.empty()) {
		return nullptr;
	}

	const auto it = mappedPlayerNames.find(asLowerCaseString(s));
	if (it == mappedPlayerNames.end()) {
		return nullptr;
	}
	return it->second;
}

PlayerPtr Game::getPlayerByGUID(const uint32_t& guid)
{
	if (guid == 0) {
		return nullptr;
	}

	const auto it = mappedPlayerGuids.find(guid);
	if (it == mappedPlayerGuids.end()) {
		return nullptr;
	}
	return it->second;
}

ReturnValue Game::getPlayerByNameWildcard(const std::string& s)
{
	bool found = false;
	size_t strlen = s.length();
	if (strlen == 0 || strlen > PLAYER_NAME_LENGTH) {
		return RETURNVALUE_PLAYERWITHTHISNAMEISNOTONLINE;
	}

	if (s.back() == '~') {
		const std::string& query = asLowerCaseString(s.substr(0, strlen - 1));
		std::string result;
		if (const ReturnValue ret = wildcardTree.findOne(query, result); ret != RETURNVALUE_NOERROR) {
			return ret;
		}

		found = getPlayerByName(result) ? true : false;
	} else {
		found = getPlayerByName(s) ? true : false;
	}

	if (!found) {
		return RETURNVALUE_PLAYERWITHTHISNAMEISNOTONLINE;
	}

	return RETURNVALUE_NOERROR;
}

PlayerPtr Game::getPlayerByAccount(const uint32_t acc)
{
	for (const auto& val : players | std::views::values) {
		if (val->getAccount() == acc) {
			return val;
		}
	}
	return nullptr;
}

bool Game::internalPlaceCreature(CreaturePtr creature, const Position& pos, bool extendedPos /*=false*/, bool forced /*= false*/)
{
	if (creature->getParent() != nullptr) {
		return false;
	}

	if (!map.placeCreature(pos, creature, extendedPos, forced)) {
		return false;
	}
	
	creature->setID();
	creature->addList();
	return true;
}

bool Game::placeCreature(CreaturePtr creature, const Position& pos, bool extendedPos /*=false*/, bool forced /*= false*/, MagicEffectClasses magicEffect /*= CONST_ME_TELEPORT*/)
{
	if (!internalPlaceCreature(creature, pos, extendedPos, forced)) {
		return false;
	}

	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true);
	for (const auto spectator : spectators) {
		if (const auto tmpPlayer = spectator->getPlayer()) {
			tmpPlayer->sendCreatureAppear(creature, creature->getPosition(), magicEffect);
		}
	}

	for (const auto spectator : spectators) {
		spectator->onCreatureAppear(creature, true);
	}

	if (creature->getParent() != nullptr)	{
		creature->getParent()->postAddNotification(creature, nullptr, 0);
	}
	addCreatureCheck(creature);
	creature->onPlacedCreature();
	return true;
}

bool Game::removeCreature(CreaturePtr creature, bool isLogout/* = true*/)
{
	if (creature->isRemoved()) {
		return false;
	}

	const auto tile = creature->getTile();

	std::vector<int32_t> oldStackPosVector;

	SpectatorVec spectators;
	map.getSpectators(spectators, tile->getPosition(), true);
	for (const auto spectator : spectators) {
		if (const auto player = spectator->getPlayer()) {
			oldStackPosVector.push_back(player->canSeeCreature(creature) ? tile->getClientIndexOfCreature(player, creature) : -1);
		}
	}

	tile->removeCreature(creature);

	const Position& tilePosition = tile->getPosition();

	//send to client
	size_t i = 0;
	for (const auto spectator : spectators) {
		if (const auto player = spectator->getPlayer()) {
			player->sendRemoveTileCreature(creature, tilePosition, oldStackPosVector[i++]);
		}
	}

	//event method
	for (const auto spectator : spectators) {
		spectator->onRemoveCreature(creature, isLogout);
	}

	const auto master = creature->getMaster();
	if (master && !master->isRemoved()) {
		creature->setMaster(nullptr);
	}

	creature->getParent()->postRemoveNotification(creature, nullptr, 0);

	creature->removeList();
	creature->setRemoved();
	// ReleaseCreature(creature);

	removeCreatureCheck(creature);

	for (auto summon : creature->summons) {
		summon->setSkillLoss(false);
		removeCreature(summon);
	}
	return true;
}

void Game::executeDeath(uint32_t creatureId)
{
	const auto creature = getCreatureByID(creatureId);
	if (creature && !creature->isRemoved()) {
		creature->onDeath();
	}
}

void Game::playerMoveThing(const uint32_t playerId, const Position& fromPos,
                           const uint16_t spriteId, const uint8_t fromStackPos, const Position& toPos, uint8_t count)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	uint8_t fromIndex = 0;
	if (fromPos.x == 0xFFFF) {
		if (fromPos.y & 0x40) {
			fromIndex = fromPos.z;
		} else {
			fromIndex = static_cast<uint8_t>(fromPos.y);
		}
	} else {
		fromIndex = fromStackPos;
	}

	auto thing = internalGetThing(player, fromPos, fromIndex, 0, STACKPOS_MOVE);
	if (!thing) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	if (auto movingCreature = thing->getCreature()) {
		auto tile = map.getTile(toPos);
		if (!tile) {
			player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
			return;
		}

		if (Position::areInRange<1, 1, 0>(movingCreature->getPosition(), player->getPosition()) && !player->isAccessPlayer() ) {
			SchedulerTask* task = createSchedulerTask(MOVE_CREATURE_INTERVAL, [=, this, playerID = player->getID(), creatureID = movingCreature->getID()]() {
				playerMoveCreatureByID(playerID, creatureID, fromPos, toPos);
				});
			player->setNextActionTask(task);
		} else {
			playerMoveCreature(player, movingCreature, movingCreature->getPosition(), tile);
		}
	} else if (auto move_item = thing->getItem()) {
		auto toCylinder = internalGetCylinder(player, toPos);
		if (!toCylinder) {
			player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
			return;
		}
		
		playerMoveItem(player, fromPos, spriteId, fromStackPos, toPos, count, move_item, toCylinder);
	}
}

void Game::playerMoveCreatureByID(const uint32_t playerId, const uint32_t movingCreatureId, const Position& movingCreatureOrigPos, const Position& toPos)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	auto movingCreature = getCreatureByID(movingCreatureId);
	if (!movingCreature) {
		return;
	}

	auto toTile = map.getTile(toPos);
	if (!toTile) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	playerMoveCreature(player, movingCreature, movingCreatureOrigPos, toTile);
}

void Game::playerMoveCreature(PlayerPtr& player, CreaturePtr& movingCreature, const Position& movingCreatureOrigPos, TilePtr& toTile)
{
	if (!player->canDoAction() && !player->isAccessPlayer()) {
		uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay,
			[=, this, playerID = player->getID(), movingCreatureID = movingCreature->getID(), toPos = toTile->getPosition()]() {
				playerMoveCreatureByID(playerID, movingCreatureID, movingCreatureOrigPos, toPos);
			});
		player->setNextActionTask(task);
		return;
	}

	if (movingCreature->isMovementBlocked()) {
		player->sendCancelMessage(RETURNVALUE_NOTMOVEABLE);
		return;
	}

	player->setNextActionTask(nullptr);

	if (player->isAccessPlayer()) {
		if ((!movingCreature->isPushable() && !player->hasFlag(PlayerFlag_CanPushAllCreatures)) ||
			(movingCreature->isInGhostMode() && !player->canSeeGhostMode(movingCreature))) {
			player->sendCancelMessage(RETURNVALUE_NOTMOVEABLE);
			return;
		}
		ReturnValue ret = internalMoveCreature(movingCreature, toTile);
		if (ret != RETURNVALUE_NOERROR) {
			player->sendCancelMessage(ret);
		}
		return;
	}

	if (!Position::areInRange<1, 1, 0>(movingCreatureOrigPos, player->getPosition())) {
		//need to walk to the creature first before moving it
		std::vector<Direction> listDir;
		if (player->getPathTo(movingCreatureOrigPos, listDir, 0, 1, true, true)) {
			g_dispatcher.addTask(createTask([=, this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
			SchedulerTask* task = createSchedulerTask(RANGE_MOVE_CREATURE_INTERVAL,
				[=, this, playerID = player->getID(), movingCreatureID = movingCreature->getID(), toPos = toTile->getPosition()] {
					playerMoveCreatureByID(playerID, movingCreatureID, movingCreatureOrigPos, toPos);
				});
			player->setNextWalkActionTask(task);
		} else {
			player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
		}
		return;
	}

	if ((!movingCreature->isPushable() && !player->hasFlag(PlayerFlag_CanPushAllCreatures)) ||
	        (movingCreature->isInGhostMode() && !player->canSeeGhostMode(movingCreature))) {
		player->sendCancelMessage(RETURNVALUE_NOTMOVEABLE);
		return;
	}

	//check throw distance
	const Position& movingCreaturePos = movingCreature->getPosition();
	const Position& toPos = toTile->getPosition();
	if ((Position::getDistanceX(movingCreaturePos, toPos) > movingCreature->getThrowRange()) || (Position::getDistanceY(movingCreaturePos, toPos) > movingCreature->getThrowRange()) || (Position::getDistanceZ(movingCreaturePos, toPos) * 4 > movingCreature->getThrowRange())) {
		player->sendCancelMessage(RETURNVALUE_DESTINATIONOUTOFREACH);
		return;
	}

	if (!Position::areInRange<1, 1, 0>(movingCreaturePos, player->getPosition())) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	if (player != movingCreature) {
		if (toTile->hasFlag(TILESTATE_BLOCKPATH)) {
			player->sendCancelMessage(RETURNVALUE_NOTENOUGHROOM);
			return;
		} else if ((movingCreature->getZone() == ZONE_PROTECTION && !toTile->hasFlag(TILESTATE_PROTECTIONZONE)) || (movingCreature->getZone() == ZONE_NOPVP && !toTile->hasFlag(TILESTATE_NOPVPZONE))) {
			player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
			return;
		} else {
			if (const auto tileCreatures = toTile->getCreatures()) {
				for (const auto tileCreature : *tileCreatures) {
					if (!tileCreature->isInGhostMode()) {
						player->sendCancelMessage(RETURNVALUE_NOTENOUGHROOM);
						return;
					}
				}
			}

			const auto movingNpc = movingCreature->getNpc();
			if (movingNpc && !Spawns::isInZone(movingNpc->getMasterPos(), movingNpc->getMasterRadius(), toPos)) {
				player->sendCancelMessage(RETURNVALUE_NOTENOUGHROOM);
				return;
			}
		}
	}

	if (!g_events->eventPlayerOnMoveCreature(player, movingCreature, movingCreaturePos, toPos)) {
		return;
	}

	ReturnValue ret = internalMoveCreature(movingCreature, toTile);
	if (ret != RETURNVALUE_NOERROR) {
		player->sendCancelMessage(ret);
	}
}

ReturnValue Game::internalMoveCreature(CreaturePtr creature, const Direction direction, uint32_t flags /*= 0*/)
{
	creature->setLastPosition(creature->getPosition());
	const Position& currentPos = creature->getPosition();
	Position destPos = getNextPosition(direction, currentPos);
	auto player = creature->getPlayer();

	bool diagonalMovement = (direction & DIRECTION_DIAGONAL_MASK) != 0;
	if (player && !diagonalMovement) {
		//try to go up
		if (currentPos.z != 8 && creature->getTile()->hasHeight(3)) {
			auto tmpTile = map.getTile(currentPos.x, currentPos.y, currentPos.getZ() - 1);
			if (tmpTile == nullptr || (tmpTile->getGround() == nullptr && !tmpTile->hasFlag(TILESTATE_BLOCKSOLID))) {
				tmpTile = map.getTile(destPos.x, destPos.y, destPos.getZ() - 1);
				if (tmpTile && tmpTile->getGround() && !tmpTile->hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID)) {
					flags |= FLAG_IGNOREBLOCKITEM | FLAG_IGNOREBLOCKCREATURE;

					if (!tmpTile->hasFlag(TILESTATE_FLOORCHANGE)) {
						player->setDirection(direction);
						destPos.z--;
					}
				}
			}
		}

		//try to go down
		if (currentPos.z != 7 && currentPos.z == destPos.z) {
			auto tmpTile = map.getTile(destPos.x, destPos.y, destPos.z);
			if (tmpTile == nullptr || (tmpTile->getGround() == nullptr && !tmpTile->hasFlag(TILESTATE_BLOCKSOLID))) {
				tmpTile = map.getTile(destPos.x, destPos.y, destPos.z + 1);
				if (tmpTile && tmpTile->hasHeight(3) && !tmpTile->hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID)) {
					flags |= FLAG_IGNOREBLOCKITEM | FLAG_IGNOREBLOCKCREATURE;
					player->setDirection(direction);
					destPos.z++;
				}
			}
		}
	}

	const auto toTile = map.getTile(destPos);
	if (!toTile) {
		return RETURNVALUE_NOTPOSSIBLE;
	}
	return internalMoveCreature(creature, toTile, flags);
}

ReturnValue Game::internalMoveCreature(CreaturePtr creature, TilePtr toTile, uint32_t flags /*= 0*/)
{
	//check if we can move the creature to the destination
	ReturnValue ret = toTile->queryAdd(creature, flags);
	if (ret != RETURNVALUE_NOERROR) {
		return ret;
	}

	map.moveCreature(creature, toTile);
    if (creature->getParent() != toTile) {
		return RETURNVALUE_NOERROR;
	}

	int32_t index = 0;
	ItemPtr toItem = nullptr;
	CylinderPtr subCylinder = nullptr;
	TilePtr toCylinder = toTile;
	TilePtr fromCylinder = nullptr;
	uint32_t n = 0;
	

	while ((subCylinder = toCylinder->queryDestination(index, creature, toItem, flags)) != toCylinder) {
		const auto subTile = std::dynamic_pointer_cast<Tile>(subCylinder);
		map.moveCreature(creature, subTile);

        if (creature->getParent() != subCylinder) {
			//could happen if a script move the creature
			fromCylinder = nullptr;
			break;
		}

		fromCylinder = toCylinder;
		toCylinder = subTile;
		flags = 0;

		//to prevent infinite loop
		if (++n >= MAP_MAX_LAYERS) {
			break;
		}
	}

	if (fromCylinder) {
		const Position& fromPosition = fromCylinder->getPosition();
		const Position& toPosition = toCylinder->getPosition();
		if (fromPosition.z != toPosition.z && (fromPosition.x != toPosition.x || fromPosition.y != toPosition.y)) {
			Direction dir = getDirectionTo(fromPosition, toPosition);
			if ((dir & DIRECTION_DIAGONAL_MASK) == 0) {
				internalCreatureTurn(creature, dir);
			}
		}
	}

	return RETURNVALUE_NOERROR;
}

void Game::playerMoveItemByPlayerID(const uint32_t playerId, const Position& fromPos, uint16_t spriteId, uint8_t fromStackPos, const Position& toPos, uint8_t count)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}
	ItemPtr item = nullptr;
	CylinderPtr toCylinder = nullptr;
	// perfect example of needing an optional
	playerMoveItem(player, fromPos, spriteId, fromStackPos, toPos, count, item, toCylinder);
}

void Game::playerMoveItem(const PlayerPtr& player, 
							const Position& fromPos,
							uint16_t spriteId,
							uint8_t fromStackPos,
							const Position& toPos,
							uint8_t count,
							ItemPtr item,
							CylinderPtr toCylinder)
{
	if (!player->canDoAction() && !player->isAccessPlayer()) {
		uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, [=, this, playerID = player->getID()]() {
			playerMoveItemByPlayerID(playerID, fromPos, spriteId, fromStackPos, toPos, count);
			});
		player->setNextActionTask(task);
		return;
	}

	player->setNextActionTask(nullptr);

	if (item == nullptr) {
		uint8_t fromIndex = 0;
		if (fromPos.x == 0xFFFF) {
			if (fromPos.y & 0x40) {
				fromIndex = fromPos.z;
			} else {
				fromIndex = static_cast<uint8_t>(fromPos.y);
			}
		} else {
			fromIndex = fromStackPos;
		}

		auto thing = internalGetThing(player, fromPos, fromIndex, 0, STACKPOS_MOVE);
		if (!thing || !thing->getItem()) {
			player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
			return;
		}
		item = thing->getItem();
	}

	if (item->getClientID() != spriteId) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	auto fromCylinder = internalGetCylinder(player, fromPos);
	if (fromCylinder == nullptr) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	if (toCylinder == nullptr) {
		toCylinder = internalGetCylinder(player, toPos);
		if (toCylinder == nullptr) {
			player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
			return;
		}
	}

	if (!item->isPushable() || item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID)) {
		player->sendCancelMessage(RETURNVALUE_NOTMOVEABLE);
		return;
	}

	if (player->isAccessPlayer()) {
		uint8_t toIndex = 0;
		if (toPos.x == 0xFFFF) {
			if (toPos.y & 0x40) {
				toIndex = toPos.z;
			}
			else {
				toIndex = static_cast<uint8_t>(toPos.y);
			}
		}

		ReturnValue ret = internalMoveItem(fromCylinder, toCylinder, toIndex, item, count, std::nullopt, 0, player, nullptr, &fromPos, &toPos);
		return;
	}

	const Position& playerPos = player->getPosition();
	const Position& mapFromPos = fromCylinder->getTile()->getPosition();
	if (playerPos.z != mapFromPos.z) {
		player->sendCancelMessage(playerPos.z > mapFromPos.z ? RETURNVALUE_FIRSTGOUPSTAIRS : RETURNVALUE_FIRSTGODOWNSTAIRS);
		return;
	}

	if (!Position::areInRange<1, 1>(playerPos, mapFromPos)) {
		//need to walk to the item first before using it
		std::vector<Direction> listDir;
		if (player->getPathTo(item->getPosition(), listDir, 0, 1, true, true)) {
			g_dispatcher.addTask(createTask([=, this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); })); // suspect lamba
			SchedulerTask* task = createSchedulerTask(RANGE_MOVE_ITEM_INTERVAL, [=, this, playerID = player->getID()]() {
				playerMoveItemByPlayerID(playerID, fromPos, spriteId, fromStackPos, toPos, count);
				});
			player->setNextWalkActionTask(task);
		} else {
			player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
		}
		return;
	}

	const auto& toCylinderTile = toCylinder->getTile();
	const Position& mapToPos = toCylinderTile->getPosition();

	//hangable item specific code
	if (item->isHangable() && toCylinderTile->hasFlag(TILESTATE_SUPPORTS_HANGABLE)) {
		//destination supports hangable objects so need to move there first
		bool vertical = toCylinderTile->hasProperty(CONST_PROP_ISVERTICAL);
		if (vertical) {
			if (playerPos.x + 1 == mapToPos.x) {
				player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
				return;
			}
		} else { // horizontal
			if (playerPos.y + 1 == mapToPos.y) {
				player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
				return;
			}
		}

		if (!Position::areInRange<1, 1, 0>(playerPos, mapToPos)) {
			Position walkPos = mapToPos;
			if (vertical) {
				walkPos.x++;
			} else {
				walkPos.y++;
			}

			Position itemPos = fromPos;
			uint8_t itemStackPos = fromStackPos;

			if (fromPos.x != 0xFFFF && Position::areInRange<1, 1>(mapFromPos, playerPos)
			        && !Position::areInRange<1, 1, 0>(mapFromPos, walkPos)) {
				//need to pickup the item first
				ItemPtr moveItem = nullptr;
				CylinderPtr p_cylinder = std::dynamic_pointer_cast<Cylinder>(player);
				ReturnValue ret = internalMoveItem(fromCylinder, p_cylinder, INDEX_WHEREEVER, item, count, std::ref(moveItem), 0, player, nullptr, &fromPos, &toPos);
				if (ret != RETURNVALUE_NOERROR) {
					player->sendCancelMessage(ret);
					return;
				}

				//changing the position since its now in the inventory of the player
				internalGetPosition(moveItem, itemPos, itemStackPos);
			}

			std::vector<Direction> listDir;
			if (player->getPathTo(walkPos, listDir, 0, 0, true, true)) {
				g_dispatcher.addTask(createTask([=, this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
				SchedulerTask* task = createSchedulerTask(RANGE_MOVE_ITEM_INTERVAL,
					[this, playerID = player->getID(), itemPos, spriteId, itemStackPos, toPos, count]() {
						playerMoveItemByPlayerID(playerID, itemPos, spriteId, itemStackPos, toPos, count);
					});
				player->setNextWalkActionTask(task);
			} else {
				player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
			}
			return;
		}
	}

	if (!item->isPickupable() && playerPos.z != mapToPos.z) {
		player->sendCancelMessage(RETURNVALUE_DESTINATIONOUTOFREACH);
		return;
	}

	int32_t throwRange = item->getThrowRange();
	if ((Position::getDistanceX(playerPos, mapToPos) > throwRange) ||
	        (Position::getDistanceY(playerPos, mapToPos) > throwRange)) {
		player->sendCancelMessage(RETURNVALUE_DESTINATIONOUTOFREACH);
		return;
	}

	if (!canThrowObjectTo(mapFromPos, mapToPos, true, false, throwRange, throwRange)) {
		player->sendCancelMessage(RETURNVALUE_CANNOTTHROW);
		return;
	}

	uint8_t toIndex = 0;
	if (toPos.x == 0xFFFF) {
		if (toPos.y & 0x40) {
			toIndex = toPos.z;
		} else {
			toIndex = static_cast<uint8_t>(toPos.y);
		}
	}

	ReturnValue ret = internalMoveItem(fromCylinder, toCylinder, toIndex, item, count, std::nullopt, 0, player, nullptr, &fromPos, &toPos);
	if (ret != RETURNVALUE_NOERROR) {
		player->sendCancelMessage(ret);
	}
}

ReturnValue Game::internalMoveItem(CylinderPtr fromCylinder,
									CylinderPtr toCylinder,
									int32_t index,
									ItemPtr item,
									uint32_t count,
									std::optional<std::reference_wrapper<ItemPtr>> _moveItem,
									uint32_t flags				/* = 0*/,
									CreaturePtr actor			/* = nullptr*/,
									ItemPtr tradeItem			/* = nullptr*/,
									const Position* fromPos		/* = nullptr*/,
									const Position* toPos		/* = nullptr*/)
{
	PlayerPtr actorPlayer =  actor ? actor->getPlayer() : nullptr;
	if (actorPlayer && fromPos && toPos) {
		const ReturnValue ret = g_events->eventPlayerOnMoveItem(actorPlayer, item, count, *fromPos, *toPos, fromCylinder, toCylinder);
		if (ret != RETURNVALUE_NOERROR) {
			return ret;
		}
	}

	if (const auto fromTile = fromCylinder->getTile()) {
		if (const auto it = browseFields.find(fromTile); it != browseFields.end() && it->second == fromCylinder) {
			fromCylinder = fromTile;
		}
	}

	ItemPtr toItem = nullptr;

	CylinderPtr subCylinder;
	int floorN = 0;

	while ((subCylinder = toCylinder->queryDestination(index, item, toItem, flags)) != toCylinder) {
		toCylinder = subCylinder;
		//to prevent infinite loop
		if (++floorN >= MAP_MAX_LAYERS) {
			break;
		}
	}

	//destination is the same as the source?
	if (item == toItem) {
		return RETURNVALUE_NOERROR; //silently ignore move
	}

	if (ContainerPtr toContainer = std::dynamic_pointer_cast<Container>(toCylinder)) {
		if (toContainer->isRewardCorpse() || toContainer->getID() == ITEM_REWARD_CONTAINER) {
			return RETURNVALUE_NOTPOSSIBLE;
		}
	}

	if (ContainerPtr itemContainer = std::dynamic_pointer_cast<Container>(item)) {
		if (itemContainer->isRewardCorpse() || item->getID() == ITEM_REWARD_CONTAINER) {
			return RETURNVALUE_NOERROR; // silently ignore move
		}
	}

	//check if we can add this item
	ReturnValue ret = toCylinder->queryAdd(index, item, count, flags, actor);
	if (ret == RETURNVALUE_NEEDEXCHANGE) {
		//check if we can add it to source cylinder
		ret = fromCylinder->queryAdd(fromCylinder->getThingIndex(item), toItem, toItem->getItemCount(), 0);
		if (ret == RETURNVALUE_NOERROR) {
			if (actorPlayer && fromPos && toPos) {
				const ReturnValue eventRet = g_events->eventPlayerOnMoveItem(actorPlayer, toItem, toItem->getItemCount(), *toPos, *fromPos, toCylinder, fromCylinder);
				if (eventRet != RETURNVALUE_NOERROR) {
					return eventRet;
				}
			}

			//check how much we can move
			uint32_t maxExchangeQueryCount = 0;
			ReturnValue retExchangeMaxCount = fromCylinder->queryMaxCount(INDEX_WHEREEVER, toItem, toItem->getItemCount(), maxExchangeQueryCount, 0);

			if (retExchangeMaxCount != RETURNVALUE_NOERROR && maxExchangeQueryCount == 0) {
				return retExchangeMaxCount;
			}

			if (toCylinder->queryRemove(toItem, toItem->getItemCount(), flags, actor) == RETURNVALUE_NOERROR) {
				int32_t oldToItemIndex = toCylinder->getThingIndex(toItem);
				toCylinder->removeThing(toItem, toItem->getItemCount());
				fromCylinder->addThing(toItem);

				if (oldToItemIndex != -1) {
					toCylinder->postRemoveNotification(toItem, fromCylinder, oldToItemIndex);
				}

				int32_t newToItemIndex = fromCylinder->getThingIndex(toItem);
				if (newToItemIndex != -1) {
					fromCylinder->postAddNotification(toItem, toCylinder, newToItemIndex);
				}

				ret = toCylinder->queryAdd(index, item, count, flags);

				if (actorPlayer && fromPos && toPos && !toItem->isRemoved()) {
					g_events->eventPlayerOnItemMoved(actorPlayer, toItem, toItem->getItemCount(), *toPos, *fromPos, toCylinder, fromCylinder);
				}

				toItem = nullptr;
			}
		}
	}

	if (ret != RETURNVALUE_NOERROR) {
		return ret;
	}

	//check how much we can move
	uint32_t maxQueryCount = 0;
	ReturnValue retMaxCount = toCylinder->queryMaxCount(index, item, count, maxQueryCount, flags);
	if (retMaxCount != RETURNVALUE_NOERROR && maxQueryCount == 0) {
		return retMaxCount;
	}

	uint32_t m;
	if (item->isStackable()) {
		m = std::min<uint32_t>(count, maxQueryCount);
	} else {
		m = maxQueryCount;
	}

	ItemPtr moveItem = item;

	//check if we can remove this item
	ret = fromCylinder->queryRemove(item, m, flags, actor);
	if (ret != RETURNVALUE_NOERROR) {
		return ret;
	}

	if (tradeItem) {
		if (toCylinder->getItem() == tradeItem) {
			return RETURNVALUE_NOTENOUGHROOM;
		}

		auto tmpCylinder = toCylinder->getParent();
		while (tmpCylinder) {
			if (tmpCylinder->getItem() == tradeItem) {
				return RETURNVALUE_NOTENOUGHROOM;
			}

			tmpCylinder = tmpCylinder->getParent();
		}
	}

	//remove the item
	int32_t itemIndex = fromCylinder->getThingIndex(item);
	ItemPtr updateItem = nullptr;
	fromCylinder->removeThing(item, m);

	//update item(s)
	if (item->isStackable()) {
		uint32_t n;

		if (item->equals(toItem)) {
			n = std::min<uint32_t>(100 - toItem->getItemCount(), m);
			toCylinder->updateThing(toItem, toItem->getID(), toItem->getItemCount() + n);
			updateItem = toItem;
		} else {
			n = 0;
		}

		int32_t newCount = m - n;
		if (newCount > 0) {
			moveItem = item->clone();
			moveItem->setItemCount(newCount);
		} else {
			moveItem = nullptr;
		}
	}

	//add item
	if (moveItem /*m - n > 0*/) {
		toCylinder->addThing(index, moveItem);
	}

	if (item && itemIndex != -1) {
		fromCylinder->postRemoveNotification(item, toCylinder, itemIndex);
	}

	if (moveItem) {
		int32_t moveItemIndex = toCylinder->getThingIndex(moveItem);
		if (moveItemIndex != -1) {
			toCylinder->postAddNotification(moveItem, fromCylinder, moveItemIndex);
		}
	}

	if (updateItem) {
		int32_t updateItemIndex = toCylinder->getThingIndex(updateItem);
		if (updateItemIndex != -1) {
			toCylinder->postAddNotification(updateItem, fromCylinder, updateItemIndex);
		}
	}

	if (_moveItem) {
		if (moveItem) {
			_moveItem.value().get() = moveItem;
		} else {
			_moveItem.value().get() = item;
		}
	}

	//we could not move all, inform the player
	if (item && item->isStackable() && maxQueryCount < count) {
		return retMaxCount;
	}

	if (moveItem && moveItem->getDuration() > 0) {
		if (moveItem->getDecaying() != DECAYING_TRUE) {
			moveItem->setDecaying(DECAYING_TRUE);
			toDecayItems.push_front(moveItem);
		}
	}

	if (actorPlayer && fromPos && toPos) {
		if (updateItem && !updateItem->isRemoved()) {
			g_events->eventPlayerOnItemMoved(actorPlayer, updateItem, count, *fromPos, *toPos, fromCylinder, toCylinder);
		} else if (moveItem && !moveItem->isRemoved()) {
			g_events->eventPlayerOnItemMoved(actorPlayer, moveItem, count, *fromPos, *toPos, fromCylinder, toCylinder);
		} else if (item && !item->isRemoved()) {
			g_events->eventPlayerOnItemMoved(actorPlayer, item, count, *fromPos, *toPos, fromCylinder, toCylinder);
		}
	}

	return ret;
}

ReturnValue Game::internalAddItem(CylinderPtr toCylinder, ItemPtr item, int32_t index /*= INDEX_WHEREEVER*/,
                                  uint32_t flags/* = 0*/, bool test/* = false*/)
{
	uint32_t remainderCount = 0;
	return internalAddItem(toCylinder, item, index, flags, test, remainderCount);
}

ReturnValue Game::internalAddItem(CylinderPtr toCylinder, ItemPtr item, int32_t index,
                                  uint32_t flags, bool test, uint32_t& remainderCount)
{
	if (toCylinder == nullptr || item == nullptr) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	auto destCylinder = toCylinder;
	ItemPtr toItem = nullptr;
	toCylinder = toCylinder->queryDestination(index, item, toItem, flags);

	//check if we can add this item
	ReturnValue ret = toCylinder->queryAdd(index, item, item->getItemCount(), flags);
	if (ret != RETURNVALUE_NOERROR) {
		return ret;
	}

	/*
	Check if we can move add the whole amount, we do this by checking against the original cylinder,
	since the queryDestination can return a cylinder that might only hold a part of the full amount.
	*/
	uint32_t maxQueryCount = 0;
	ret = destCylinder->queryMaxCount(INDEX_WHEREEVER, item, item->getItemCount(), maxQueryCount, flags);

	if (ret != RETURNVALUE_NOERROR) {
		return ret;
	}

	if (test) {
		return RETURNVALUE_NOERROR;
	}

	if (item->isStackable() && item->equals(toItem)) {
		uint32_t m = std::min<uint32_t>(item->getItemCount(), maxQueryCount);
		uint32_t n = std::min<uint32_t>(100 - toItem->getItemCount(), m);

		toCylinder->updateThing(toItem, toItem->getID(), toItem->getItemCount() + n);

		int32_t count = m - n;
		if (count > 0) {
			if (item->getItemCount() != count) {
				auto remainderItem = item->clone();
				remainderItem->setItemCount(count);
				if (internalAddItem(destCylinder, remainderItem, INDEX_WHEREEVER, flags, false) != RETURNVALUE_NOERROR) {
					// ReleaseItem(remainderItem);
					remainderCount = count;
				}
			} else {
				toCylinder->addThing(index, item);

				int32_t itemIndex = toCylinder->getThingIndex(item);
				if (itemIndex != -1) {
					toCylinder->postAddNotification(item, nullptr, itemIndex);
				}
			}
		} else {
			//fully merged with toItem, item will be destroyed
			item->onRemoved();
			// ReleaseItem(item);

			int32_t itemIndex = toCylinder->getThingIndex(toItem);
			if (itemIndex != -1) {
				toCylinder->postAddNotification(toItem, nullptr, itemIndex);
			}
		}
	} else {
		toCylinder->addThing(index, item);

		int32_t itemIndex = toCylinder->getThingIndex(item);
		if (itemIndex != -1) {
			toCylinder->postAddNotification(item, nullptr, itemIndex);
		}
	}

	if (item->getDuration() > 0) {
		item->setDecaying(DECAYING_TRUE);
		toDecayItems.push_front(item);
	}
	return RETURNVALUE_NOERROR;
}

ReturnValue Game::internalRemoveItem(ItemPtr item, int32_t count /*= -1*/, bool test /*= false*/, uint32_t flags /*= 0*/)
{
	auto cylinder = item->getParent();
	if (cylinder == nullptr) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (auto fromTile = cylinder->getTile()) {
		if (auto it = browseFields.find(fromTile); it != browseFields.end() && it->second == cylinder) {
			cylinder = fromTile;
		}
	}

	if (count == -1) {
		count = item->getItemCount();
	}

	//check if we can remove this item
	ReturnValue ret = cylinder->queryRemove(item, count, flags | FLAG_IGNORENOTMOVEABLE);
	if (ret != RETURNVALUE_NOERROR) {
		return ret;
	}

	if (!item->canRemove()) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (!test) {
		int32_t index = cylinder->getThingIndex(item);
		//remove the item
		cylinder->removeThing(item, count);

		if (item->isRemoved()) {
			item->onRemoved();
			if (item->canDecay()) {
				decayItems->remove(item);
			}
		}
		cylinder->postRemoveNotification(item, nullptr, index);
	}

	return RETURNVALUE_NOERROR;
}

ReturnValue Game::internalPlayerAddItem(const PlayerPtr& player, ItemPtr item, bool dropOnMap /*= true*/, slots_t slot /*= CONST_SLOT_WHEREEVER*/)
{
	uint32_t remainderCount = 0;
	CylinderPtr holder = std::static_pointer_cast<Cylinder>(player);
	ReturnValue ret = internalAddItem(holder, item, static_cast<int32_t>(slot), 0, false, remainderCount);
	if (remainderCount != 0) {
		auto remainderItem = Item::CreateItem(item->getID(), remainderCount);
		CylinderPtr tile = player->getTile();
		ReturnValue remaindRet = internalAddItem(tile, remainderItem, INDEX_WHEREEVER, FLAG_NOLIMIT);
		if (remaindRet != RETURNVALUE_NOERROR) {
			// ReleaseItem(remainderItem);
		}
	}

	if (ret != RETURNVALUE_NOERROR && dropOnMap) {
		CylinderPtr tile = player->getTile();
		ret = internalAddItem(tile, item, INDEX_WHEREEVER, FLAG_NOLIMIT);
	}

	return ret;
}

ItemPtr Game::findItemOfType(const CylinderPtr& cylinder, uint16_t itemId,
                           bool depthSearch /*= true*/, int32_t subType /*= -1*/) const
{
	if (cylinder == nullptr) {
		return nullptr;
	}

	std::vector<ContainerPtr> containers;
	for (size_t i = cylinder->getFirstIndex(), j = cylinder->getLastIndex(); i < j; ++i) {
		auto thing = cylinder->getThing(i);
		if (!thing) {
			continue;
		}

		auto item = thing->getItem();
		if (!item) {
			continue;
		}

		if (item->getID() == itemId && (subType == -1 || subType == item->getSubType())) {
			return item;
		}

		if (depthSearch) {
			if (auto container = item->getContainer()) {
				containers.push_back(container);
			}
		}
	}

	size_t i = 0;
	while (i < containers.size()) {
		for (const auto container = containers[i++]; auto item : container->getItemList()) {
			if (item->getID() == itemId && (subType == -1 || subType == item->getSubType())) {
				return item;
			}

			if (auto subContainer = item->getContainer()) {
				containers.push_back(subContainer);
			}
		}
	}
	return nullptr;
}

bool Game::removeMoney(CylinderPtr& cylinder, uint64_t money, uint32_t flags /*= 0*/)
{
	if (cylinder == nullptr) {
		return false;
	}

	if (money == 0) {
		return true;
	}

	std::vector<ContainerPtr> containers;

	std::multimap<uint32_t, ItemPtr> moneyMap;
	uint64_t moneyCount = 0;

	for (size_t i = cylinder->getFirstIndex(), j = cylinder->getLastIndex(); i < j; ++i) {
		auto thing = cylinder->getThing(i);
		if (!thing) {
			continue;
		}

		auto item = thing->getItem();
		if (!item) {
			continue;
		}

		if (auto container = item->getContainer()) {
			containers.push_back(container);
		} else {
			const uint32_t worth = item->getWorth();
			if (worth != 0) {
				moneyCount += worth;
				moneyMap.emplace(worth, item);
			}
		}
	}

	size_t i = 0;
	while (i < containers.size()) {
		for (const auto container = containers[i++]; auto item : container->getItemList()) {
			if (auto tmpContainer = item->getContainer()) {
				containers.push_back(tmpContainer);
			} else {
				const uint32_t worth = item->getWorth();
				if (worth != 0) {
					moneyCount += worth;
					moneyMap.emplace(worth, item);
				}
			}
		}
	}

	if (moneyCount < money) {
		return false;
	}

	for (const auto& moneyEntry : moneyMap) {
		auto item = moneyEntry.second;
		if (moneyEntry.first < money) {
			internalRemoveItem(item);
			money -= moneyEntry.first;
		} else if (moneyEntry.first > money) {
			const uint32_t worth = moneyEntry.first / item->getItemCount();
			const uint32_t removeCount = std::ceil(money / static_cast<double>(worth));

			addMoney(cylinder, (worth * removeCount) - money, flags);
			internalRemoveItem(item, removeCount);
			break;
		} else {
			internalRemoveItem(item);
			break;
		}
	}
	return true;
}

void Game::addMoney(CylinderPtr& cylinder, uint64_t money, const uint32_t flags /*= 0*/)
{
	if (money == 0) {
		return;
	}

	for (const auto& it : Item::items.currencyItems) {
		const uint64_t worth = it.first;

		uint32_t currencyCoins = money / worth;
		if (currencyCoins <= 0) {
			continue;
		}

		money -= currencyCoins * worth;
		while (currencyCoins > 0) {
			const uint16_t count = std::min<uint32_t>(100, currencyCoins);

			auto remaindItem = Item::CreateItem(it.second, count);

			ReturnValue ret = internalAddItem(cylinder, remaindItem, INDEX_WHEREEVER, flags);
			if (ret != RETURNVALUE_NOERROR) {
				CylinderPtr cylinder_tile = cylinder->getTile();
				internalAddItem(cylinder_tile, remaindItem, INDEX_WHEREEVER, FLAG_NOLIMIT);
			}

			currencyCoins -= count;
		}
	}
}

ItemPtr Game::transformItem(const ItemPtr& item, const uint16_t newId, const int32_t newCount /*= -1*/)
{
	if (item->getID() == newId && (newCount == -1 || (newCount == item->getSubType() && newCount != 0))) { //chargeless item placed on map = infinite
		return item;
	}

	if (item->isAugmented() || item->hasImbuements()) {
		std::cout << "Warning! Attempted to transform imbued/augmented item : " << item->getName() << " \n";
		return item;
	}

	auto cylinder = item->getParent();
	if (cylinder == nullptr) {
		return nullptr;
	}

	if (auto fromTile = cylinder->getTile()) {
		if (auto it = browseFields.find(fromTile); it != browseFields.end() && it->second == cylinder) {
			cylinder = fromTile;
		}
	}

	int32_t itemIndex = cylinder->getThingIndex(item);
	if (itemIndex == -1) {
		return item;
	}

	if (!item->canTransform()) {
		return item;
	}

	const ItemType& newType = Item::items[newId];
	if (newType.id == 0) {
		return item;
	}

	const ItemType& curType = Item::items[item->getID()];
	if (curType.alwaysOnTop != newType.alwaysOnTop) {
		//This only occurs when you transform items on tiles from a downItem to a topItem (or vice versa)
		//Remove the old, and add the new
		cylinder->removeThing(item, item->getItemCount());
		cylinder->postRemoveNotification(item, cylinder, itemIndex);

		item->setID(newId);
		if (newCount != -1) {
			item->setSubType(newCount);
		}
		cylinder->addThing(item);

		auto newParent = item->getParent();
		if (newParent == nullptr) {
			// ReleaseItem(item);
			return nullptr;
		}

		newParent->postAddNotification(item, cylinder, newParent->getThingIndex(item));
		return item;
	}

	if (curType.type == newType.type) {
		//Both items has the same type so we can safely change id/subtype
		if (newCount == 0 && (item->isStackable() || item->hasAttribute(ITEM_ATTRIBUTE_CHARGES))) {
			if (item->isStackable()) {
				internalRemoveItem(item);
				return nullptr;
			} else {
				int32_t newItemId = newId;
				if (curType.id == newType.id) {
					newItemId = item->getDecayTo();
				}

				if (newItemId < 0) {
					internalRemoveItem(item);
					return nullptr;
				} else if (newItemId != newId) {
					//Replacing the the old item with the new while maintaining the old position
					ItemPtr newItem = Item::CreateItem(newItemId, 1);
					if (newItem == nullptr) {
						return nullptr;
					}

					cylinder->replaceThing(itemIndex, newItem);
					cylinder->postAddNotification(newItem, cylinder, itemIndex);

					item->clearParent();
					cylinder->postRemoveNotification(item, cylinder, itemIndex);
					// ReleaseItem(item);
					return newItem;
				} else {
					return transformItem(item, newItemId);
				}
			}
		} else {
			cylinder->postRemoveNotification(item, cylinder, itemIndex);
			uint16_t itemId = item->getID();
			int32_t count = item->getSubType();

			if (curType.id != newType.id) {
				if (newType.group != curType.group) {
					item->setDefaultSubtype();
				}

				itemId = newId;
			}

			if (newCount != -1 && newType.hasSubType()) {
				count = newCount;
			}

			cylinder->updateThing(item, itemId, count);
			cylinder->postAddNotification(item, cylinder, itemIndex);
			return item;
		}
	}

	//Replacing the old item with the new while maintaining the old position
	ItemPtr newItem;
	if (newCount == -1) {
		newItem = Item::CreateItem(newId);
	} else {
		newItem = Item::CreateItem(newId, newCount);
	}

	if (newItem == nullptr) {
		return nullptr;
	}

	cylinder->replaceThing(itemIndex, newItem);
	cylinder->postAddNotification(newItem, cylinder, itemIndex);

	item->clearParent();
	cylinder->postRemoveNotification(item, cylinder, itemIndex);
	// ReleaseItem(item);

	if (newItem->getDuration() > 0) {
		if (newItem->getDecaying() != DECAYING_TRUE) {
			newItem->setDecaying(DECAYING_TRUE);
			toDecayItems.push_front(newItem);
		}
	}

	return newItem;
}

ReturnValue Game::internalTeleport(const ThingPtr& thing, const Position& newPos, bool pushMove/* = true*/, uint32_t flags /*= 0*/)
{
	if (newPos == thing->getPosition()) {
		return RETURNVALUE_NOERROR;
	} else if (thing->isRemoved()) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	const TilePtr toTile = map.getTile(newPos);
	CylinderPtr toCylinder = toTile;
	if (!toTile) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (auto creature = thing->getCreature()) {
		if (ReturnValue ret = toTile->queryAdd(creature, FLAG_NOLIMIT); ret != RETURNVALUE_NOERROR) {
			return ret;
		}

		map.moveCreature(creature, toTile, !pushMove);
		return RETURNVALUE_NOERROR;
	} else if (const auto item = thing->getItem()) {
		auto t_parent = item->getParent();
		return internalMoveItem(t_parent, toCylinder, INDEX_WHEREEVER, item, item->getItemCount(), std::nullopt, flags);
	}
	return RETURNVALUE_NOTPOSSIBLE;
}

ItemPtr searchForItem(const ContainerPtr& container, const uint16_t itemId)
{
	for (ContainerIterator it = container->iterator(); it.hasNext(); it.advance()) {
		if ((*it)->getID() == itemId) {
			return *it;
		}
	}

	return nullptr;
}

slots_t getSlotType(const ItemType& it)
{
	slots_t slot = CONST_SLOT_RIGHT;
	if (it.weaponType != WeaponType_t::WEAPON_SHIELD) {
		int32_t slotPosition = it.slotPosition;

		if (slotPosition & SLOTP_HEAD) {
			slot = CONST_SLOT_HEAD;
		} else if (slotPosition & SLOTP_NECKLACE) {
			slot = CONST_SLOT_NECKLACE;
		} else if (slotPosition & SLOTP_ARMOR) {
			slot = CONST_SLOT_ARMOR;
		} else if (slotPosition & SLOTP_LEGS) {
			slot = CONST_SLOT_LEGS;
		} else if (slotPosition & SLOTP_FEET) {
			slot = CONST_SLOT_FEET;
		} else if (slotPosition & SLOTP_RING) {
			slot = CONST_SLOT_RING;
		} else if (slotPosition & SLOTP_AMMO) {
			slot = CONST_SLOT_AMMO;
		} else if (slotPosition & SLOTP_TWO_HAND || slotPosition & SLOTP_LEFT) {
			slot = CONST_SLOT_LEFT;
		}
	}

	return slot;
}

//Implementation of player invoked events
void Game::playerEquipItem(const uint32_t playerId, const uint16_t spriteId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto item = player->getInventoryItem(CONST_SLOT_BACKPACK);
	if (!item) {
		return;
	}

	const auto backpack = item->getContainer();
	if (!backpack) {
		return;
	}

	const ItemType& it = Item::items.getItemIdByClientId(spriteId);
	const slots_t slot = getSlotType(it);

	const auto slotItem = player->getInventoryItem(slot);
	const auto equipItem = searchForItem(backpack, it.id);

	Position fromPos, toPos;
	uint8_t fromStackPos, toStackPos;
	if (slotItem) {
		internalGetPosition(slotItem, toPos, toStackPos);
	}

	if (equipItem) {
		internalGetPosition(equipItem, fromPos, fromStackPos);
	}

	if (slotItem && slotItem->getID() == it.id && (!it.stackable || slotItem->getItemCount() == 100 || !equipItem)) {
		CylinderPtr t_slot = slotItem->getParent();
		CylinderPtr p_slot = player;
		internalMoveItem(t_slot, p_slot, CONST_SLOT_WHEREEVER, slotItem, slotItem->getItemCount(), std::nullopt, 0, player, nullptr, &fromPos, &toPos);
	} else if (equipItem) {
		CylinderPtr t_slot = equipItem->getParent();
		CylinderPtr p_slot = player;
		internalMoveItem(t_slot, p_slot, slot, equipItem, equipItem->getItemCount(), std::nullopt, 0, player, nullptr, &fromPos, &toPos);
	}
}

void Game::playerMove(const uint32_t playerId, const Direction direction)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (player->isMovementBlocked()) {
		player->sendCancelWalk();
		return;
	}

	player->resetIdleTime();
	player->setNextWalkActionTask(nullptr);

	player->startAutoWalk(direction);
}

// for account manager
void Game::playerCancelMove(uint32_t playerId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->sendCancelWalk();
}

bool Game::playerBroadcastMessage(const PlayerPtr& player, const std::string& text) const
{
	if (!player->hasFlag(PlayerFlag_CanBroadcast)) {
		return false;
	}

	std::cout << "> " << player->getName() << " broadcasted: \"" << text << "\"." << std::endl;

	for (const auto& val : players | std::views::values) {
		val->sendPrivateMessage(player, TALKTYPE_BROADCAST, text);
	}

	return true;
}

void Game::playerCreatePrivateChannel(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player || !player->isPremium()) {
		return;
	}

	ChatChannel* channel = g_chat->createChannel(player, CHANNEL_PRIVATE);
	if (!channel || !channel->addUser(player)) {
		return;
	}

	player->sendCreatePrivateChannel(channel->getId(), channel->getName());
}

void Game::playerChannelInvite(const uint32_t playerId, const std::string& name)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	PrivateChatChannel* channel = g_chat->getPrivateChannel(player);
	if (!channel) {
		return;
	}

	const auto invitePlayer = getPlayerByName(name);
	if (!invitePlayer) {
		return;
	}

	if (player == invitePlayer) {
		return;
	}

	channel->invitePlayer(player, invitePlayer);
}

void Game::playerChannelExclude(const uint32_t playerId, const std::string& name)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	PrivateChatChannel* channel = g_chat->getPrivateChannel(player);
	if (!channel) {
		return;
	}

	const auto excludePlayer = getPlayerByName(name);
	if (!excludePlayer) {
		return;
	}

	if (player == excludePlayer) {
		return;
	}

	channel->excludePlayer(player, excludePlayer);
}

void Game::playerRequestChannels(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->sendChannelsDialog();
}

void Game::playerOpenChannel(const uint32_t playerId, uint16_t channelId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	ChatChannel* channel = g_chat->addUserToChannel(player, channelId);
	if (!channel) {
		return;
	}

	const InvitedMap* invitedUsers = channel->getInvitedUsers();
	const UsersMap* users;
	if (!channel->isPublicChannel()) {
		users = &channel->getUsers();
	} else {
		users = nullptr;
	}

	player->sendChannel(channel->getId(), channel->getName(), users, invitedUsers);
}

void Game::playerCloseChannel(const uint32_t playerId, uint16_t channelId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	g_chat->removeUserFromChannel(player, channelId);
}

void Game::playerOpenPrivateChannel(const uint32_t playerId, std::string receiver)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!IOLoginData::formatPlayerName(receiver)) {
		player->sendCancelMessage("A player with this name does not exist.");
		return;
	}

	if (player->getName() == receiver) {
		player->sendCancelMessage("You cannot set up a private message channel with yourself.");
		return;
	}

	player->sendOpenPrivateChannel(receiver);
}

void Game::playerCloseNpcChannel(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	SpectatorVec spectators;
	map.getSpectators(spectators, player->getPosition());
	for (const auto spectator : spectators) {
		if (const auto npc = spectator->getNpc()) {
			npc->onPlayerCloseChannel(player);
		}
	}
}

void Game::playerReceivePing(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->receivePing();
}

void Game::playerReceivePingBack(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->sendPingBack();
}

void Game::playerAutoWalk(const uint32_t playerId, const std::vector<Direction>& listDir)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->resetIdleTime();
	player->startAutoWalk(listDir);
}

void Game::playerStopAutoWalk(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->stopWalk();
}

void Game::playerUseItemEx(const uint32_t playerId, const Position& fromPos, const uint8_t fromStackPos, const uint16_t fromSpriteId,
                           const Position& toPos, const uint8_t toStackPos, const uint16_t toSpriteId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const bool isHotkey = (fromPos.x == 0xFFFF && fromPos.y == 0 && fromPos.z == 0);
	if (isHotkey && !g_config.getBoolean(ConfigManager::AIMBOT_HOTKEY_ENABLED)) {
		return;
	}

	const auto thing = internalGetThing(player, fromPos, fromStackPos, fromSpriteId, STACKPOS_USEITEM);
	if (!thing) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	const auto item = thing->getItem();
	if (!item || !item->isUseable() || item->getClientID() != fromSpriteId) {
		player->sendCancelMessage(RETURNVALUE_CANNOTUSETHISOBJECT);
		return;
	}

	Position walkToPos = fromPos;
	ReturnValue ret = g_actions->canUse(player, fromPos);
	if (ret == RETURNVALUE_NOERROR) {
		ret = g_actions->canUse(player, toPos, item);
		if (ret == RETURNVALUE_TOOFARAWAY) {
			walkToPos = toPos;
		}
	}

	if (ret != RETURNVALUE_NOERROR) {
		if (ret == RETURNVALUE_TOOFARAWAY) {
			Position itemPos = fromPos;
			uint8_t itemStackPos = fromStackPos;

			if (fromPos.x != 0xFFFF && toPos.x != 0xFFFF && Position::areInRange<1, 1, 0>(fromPos, player->getPosition()) &&
			        !Position::areInRange<1, 1, 0>(fromPos, toPos)) {
				ItemPtr moveItem = nullptr;
				CylinderPtr t_cylinder = item->getParent();
				CylinderPtr p_cylinder = player;

				ret = internalMoveItem(t_cylinder, p_cylinder, INDEX_WHEREEVER, item, item->getItemCount(), std::ref(moveItem), 0, player, nullptr, &fromPos, &toPos);
				if (ret != RETURNVALUE_NOERROR) {
					player->sendCancelMessage(ret);
					return;
				}

				if (!moveItem) {
					// to-do put a logger here, as this should never happen.
					player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
					return;
				}

				//changing the position since its now in the inventory of the player
				internalGetPosition(moveItem, itemPos, itemStackPos);
			}

			std::vector<Direction> listDir;
			if (player->getPathTo(walkToPos, listDir, 0, 1, true, true)) {
				g_dispatcher.addTask(createTask([=, this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
				SchedulerTask* task = createSchedulerTask(RANGE_USE_ITEM_EX_INTERVAL, [=, this]() {
					playerUseItemEx(playerId, itemPos, itemStackPos, fromSpriteId, toPos, toStackPos, toSpriteId);
					});
				player->setNextWalkActionTask(task);
			} else {
				player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
			}
			return;
		}

		player->sendCancelMessage(ret);
		return;
	}

	if (!player->canDoAction()) {
		const uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, [=, this]() {
			playerUseItemEx(playerId, fromPos, fromStackPos, fromSpriteId, toPos, toStackPos, toSpriteId);
			});
		player->setNextActionTask(task);
		return;
	}

	player->resetIdleTime();
	player->setNextActionTask(nullptr);

	g_actions->useItemEx(player, fromPos, toPos, toStackPos, item, isHotkey);
}

void Game::playerUseItem(const uint32_t playerId, const Position& pos, const uint8_t stackPos,
                         const uint8_t index, const uint16_t spriteId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const bool isHotkey = (pos.x == 0xFFFF && pos.y == 0 && pos.z == 0);
	if (isHotkey && !g_config.getBoolean(ConfigManager::AIMBOT_HOTKEY_ENABLED)) {
		return;
	}

	const auto thing = internalGetThing(player, pos, stackPos, spriteId, STACKPOS_USEITEM);
	if (!thing) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	const auto item = thing->getItem();
	if (!item || item->isUseable() || item->getClientID() != spriteId) {
		player->sendCancelMessage(RETURNVALUE_CANNOTUSETHISOBJECT);
		return;
	}

	if (ReturnValue ret = g_actions->canUse(player, pos); ret != RETURNVALUE_NOERROR) {
		if (ret == RETURNVALUE_TOOFARAWAY) {
			if (std::vector<Direction> listDir; player->getPathTo(pos, listDir, 0, 1, true, true)) {
				g_dispatcher.addTask(createTask([this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
				SchedulerTask* task = createSchedulerTask(RANGE_USE_ITEM_INTERVAL, [=, this]() { playerUseItem(playerId, pos, stackPos, index, spriteId); });
				player->setNextWalkActionTask(task);
				return;
			}

			ret = RETURNVALUE_THEREISNOWAY;
		}

		player->sendCancelMessage(ret);
		return;
	}

	if (!player->canDoAction()) {
		const uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, [=, this]() { playerUseItem(playerId, pos, stackPos, index, spriteId); });
		player->setNextActionTask(task);
		return;
	}

	player->resetIdleTime();
	player->setNextActionTask(nullptr);

	g_actions->useItem(player, pos, index, item, isHotkey);
}

void Game::playerUseWithCreature(const uint32_t playerId, const Position& fromPos, const uint8_t fromStackPos, const uint32_t creatureId, const uint16_t spriteId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto creature = getCreatureByID(creatureId);
	if (!creature) {
		return;
	}

	if (!Position::areInRange<7, 5, 0>(creature->getPosition(), player->getPosition())) {
		return;
	}

	const bool isHotkey = (fromPos.x == 0xFFFF && fromPos.y == 0 && fromPos.z == 0);
	if (!g_config.getBoolean(ConfigManager::AIMBOT_HOTKEY_ENABLED)) {
		if (creature->getPlayer() || isHotkey) {
			player->sendCancelMessage(RETURNVALUE_DIRECTPLAYERSHOOT);
			return;
		}
	}

	const auto thing = internalGetThing(player, fromPos, fromStackPos, spriteId, STACKPOS_USEITEM);
	if (!thing) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	const auto item = thing->getItem();
	if (!item || !item->isUseable() || item->getClientID() != spriteId) {
		player->sendCancelMessage(RETURNVALUE_CANNOTUSETHISOBJECT);
		return;
	}

	const Position toPos = creature->getPosition();
	Position walkToPos = fromPos;
	ReturnValue ret = g_actions->canUse(player, fromPos);
	if (ret == RETURNVALUE_NOERROR) {
		ret = g_actions->canUse(player, toPos, item);
		if (ret == RETURNVALUE_TOOFARAWAY) {
			walkToPos = toPos;
		}
	}

	if (ret != RETURNVALUE_NOERROR) {
		if (ret == RETURNVALUE_TOOFARAWAY) {
			Position itemPos = fromPos;
			uint8_t itemStackPos = fromStackPos;

			if (fromPos.x != 0xFFFF && Position::areInRange<1, 1, 0>(fromPos, player->getPosition()) && !Position::areInRange<1, 1, 0>(fromPos, toPos)) {
				ItemPtr moveItem = nullptr;
				CylinderPtr t_cylinder = item->getParent();
				CylinderPtr p_cylinder = player;
				
				ret = internalMoveItem(t_cylinder, p_cylinder, INDEX_WHEREEVER, item, item->getItemCount(), std::ref(moveItem), 0, player, nullptr, &fromPos, &toPos);
				if (ret != RETURNVALUE_NOERROR) {
					player->sendCancelMessage(ret);
					return;
				}

				//changing the position since its now in the inventory of the player
				internalGetPosition(moveItem, itemPos, itemStackPos);
			}

			if (std::vector<Direction> listDir; player->getPathTo(walkToPos, listDir, 0, 1, true, true)) {
				g_dispatcher.addTask(createTask([this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
				SchedulerTask* task = createSchedulerTask(RANGE_USE_WITH_CREATURE_INTERVAL, [=, this]() {
					playerUseWithCreature(playerId, itemPos, itemStackPos, creatureId, spriteId);
					});
				player->setNextWalkActionTask(task);
			} else {
				player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
			}
			return;
		}

		player->sendCancelMessage(ret);
		return;
	}

	if (!player->canDoAction()) {
		const uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, [=, this]() { playerUseWithCreature(playerId, fromPos, fromStackPos, creatureId, spriteId); });
		player->setNextActionTask(task);
		return;
	}

	player->resetIdleTime();
	player->setNextActionTask(nullptr);

	g_actions->useItemEx(player, fromPos, creature->getPosition(), creature->getParent()->getThingIndex(creature), item, isHotkey, creature);
}

void Game::playerCloseContainer(const uint32_t playerId, const uint8_t cid)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->closeContainer(cid);
	player->sendCloseContainer(cid);
}

void Game::playerMoveUpContainer(const uint32_t playerId, uint8_t cid)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto container = player->getContainerByID(cid);
	if (!container) {
		return;
	}

	auto parentContainer = std::dynamic_pointer_cast<Container>(container->getRealParent());
	if (!parentContainer) {
		const auto tile = container->getTile();
		if (!tile) {
			return;
		}

		if (!g_events->eventPlayerOnBrowseField(player, tile->getPosition())) {
			return;
		}

		if (const auto it = browseFields.find(tile); it == browseFields.end()) {
			parentContainer = std::make_shared<Container>(tile);
			browseFields[tile] = parentContainer;
			parentContainer->setParentToTileItems(tile);
		} else {
			parentContainer = it->second;
			parentContainer->setParentToTileItems(tile);
		}
	}

	player->addContainer(cid, parentContainer);
	player->sendContainer(cid, parentContainer, parentContainer->hasParent(), player->getContainerIndex(cid));
}

void Game::playerUpdateContainer(const uint32_t playerId, uint8_t cid)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	auto container = player->getContainerByID(cid);
	if (!container) {
		return;
	}

	player->sendContainer(cid, container, container->hasParent(), player->getContainerIndex(cid));
}

void Game::playerRotateItem(const uint32_t playerId, const Position& pos, const uint8_t stackPos, const uint16_t spriteId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto thing = internalGetThing(player, pos, stackPos, 0, STACKPOS_TOPDOWN_ITEM);
	if (!thing) {
		return;
	}

	const auto item = thing->getItem();
	if (!item || item->getClientID() != spriteId || !item->isRotatable() || item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID)) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	if (pos.x != 0xFFFF && !Position::areInRange<1, 1, 0>(pos, player->getPosition())) {
		if (std::vector<Direction> listDir; player->getPathTo(pos, listDir, 0, 1, true, true)) {
			g_dispatcher.addTask(createTask([this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
			SchedulerTask* task = createSchedulerTask(RANGE_ROTATE_ITEM_INTERVAL, [=, this]() { playerRotateItem(playerId, pos, stackPos, spriteId); });
			player->setNextWalkActionTask(task);
		} else {
			player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
		}
		return;
	}

	if (const uint16_t newId = Item::items[item->getID()].rotateTo; newId != 0) {
		g_events->eventPlayerOnRotateItem(player, item);
	}
}

void Game::playerWriteItem(const uint32_t playerId, const uint32_t windowTextId, const std::string& text)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	uint16_t maxTextLength = 0;
	uint32_t internalWindowTextId = 0;

	const auto writeItem = player->getWriteItem(internalWindowTextId, maxTextLength);
	if (text.length() > maxTextLength || windowTextId != internalWindowTextId) {
		return;
	}

	if (!writeItem || writeItem->isRemoved()) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	const auto topParent = writeItem->getTopParent();

	if (const auto owner = std::dynamic_pointer_cast<Player>(topParent); owner && owner != player) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	if (!Position::areInRange<1, 1, 0>(writeItem->getPosition(), player->getPosition())) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	for (const auto creatureEvent : player->getCreatureEvents(CREATURE_EVENT_TEXTEDIT)) {
		if (!creatureEvent->executeTextEdit(player, writeItem, text)) {
			player->setWriteItem(nullptr);
			return;
		}
	}

	if (!text.empty()) {
		if (writeItem->getText() != text) {
			writeItem->setText(text);
			writeItem->setWriter(player->getName());
			writeItem->setDate(time(nullptr));
		}
	} else {
		writeItem->resetText();
		writeItem->resetWriter();
		writeItem->resetDate();
	}

	uint16_t newId = Item::items[writeItem->getID()].writeOnceItemId;
	if (newId != 0) {
		transformItem(writeItem, newId);
	}

	player->setWriteItem(nullptr);
}

void Game::playerBrowseField(const uint32_t playerId, const Position& pos)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const Position& playerPos = player->getPosition();
	if (playerPos.z != pos.z) {
		player->sendCancelMessage(playerPos.z > pos.z ? RETURNVALUE_FIRSTGOUPSTAIRS : RETURNVALUE_FIRSTGODOWNSTAIRS);
		return;
	}

	if (!Position::areInRange<1, 1>(playerPos, pos)) {
		if (std::vector<Direction> listDir; player->getPathTo(pos, listDir, 0, 1, true, true)) {
			g_dispatcher.addTask(createTask([this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
			SchedulerTask* task = createSchedulerTask(RANGE_BROWSE_FIELD_INTERVAL, [=, this]() { playerBrowseField(playerId, pos); });
			player->setNextWalkActionTask(task);
		} else {
			player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
		}
		return;
	}

	const auto tile = map.getTile(pos);
	if (!tile) {
		return;
	}

	if (!g_events->eventPlayerOnBrowseField(player, pos)) {
		return;
	}

	ContainerPtr container;

	if (const auto it = browseFields.find(tile); it == browseFields.end()) {
		container = std::make_shared<Container>(tile);
		browseFields[tile] = container;
		container->setParentToTileItems(tile);
	} else {
		container = it->second;
		container->setParentToTileItems(tile);
	}

	const uint8_t dummyContainerId = 0xF - ((pos.x % 3) * 3 + (pos.y % 3));
	if (const auto openContainer = player->getContainerByID(dummyContainerId)) {
		player->onCloseContainer(openContainer);
		player->closeContainer(dummyContainerId);
	} else {
		player->addContainer(dummyContainerId, container);
		player->sendContainer(dummyContainerId, container, false, 0);
	}
}

void Game::playerSeekInContainer(const uint32_t playerId, uint8_t containerId, uint16_t index)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto container = player->getContainerByID(containerId);
	if (!container || !container->hasPagination()) {
		return;
	}

	if ((index % container->capacity()) != 0 || index >= container->size()) {
		return;
	}

	player->setContainerIndex(containerId, index);
	player->sendContainer(containerId, container, container->hasParent(), index);
}

void Game::playerUpdateHouseWindow(const uint32_t playerId, const uint8_t listId, const uint32_t windowTextId, const std::string& text)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	uint32_t internalWindowTextId;
	uint32_t internalListId;

	auto house = player->getEditHouse(internalWindowTextId, internalListId);
	if (house && house->canEditAccessList(internalListId, player) && internalWindowTextId == windowTextId && listId == 0) {
		house->setAccessList(internalListId, text);
	}

	player->setEditHouse(nullptr);
}

void Game::playerWrapItem(const uint32_t playerId, const Position& position, const uint8_t stackPos, const uint16_t spriteId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto thing = internalGetThing(player, position, stackPos, 0, STACKPOS_TOPDOWN_ITEM);
	if (!thing) {
		return;
	}

	const auto item = thing->getItem();
	if (!item || item->getClientID() != spriteId || !item->hasAttribute(ITEM_ATTRIBUTE_WRAPID) || item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID)) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	if (position.x != 0xFFFF && !Position::areInRange<1, 1, 0>(position, player->getPosition())) {
		if (std::vector<Direction> listDir; player->getPathTo(position, listDir, 0, 1, true, true)) {
			g_dispatcher.addTask(createTask([this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
			const auto task = createSchedulerTask(RANGE_WRAP_ITEM_INTERVAL, [=, this]() { playerWrapItem(playerId, position, stackPos, spriteId); });
			player->setNextWalkActionTask(task);
		} else {
			player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
		}
		return;
	}

	g_events->eventPlayerOnWrapItem(player, item);
}

void Game::playerRequestTrade(const uint32_t playerId, const Position& pos, uint8_t stackPos,
                              const uint32_t tradePlayerId, const uint16_t spriteId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto tradePartner = getPlayerByID(tradePlayerId);
	if (!tradePartner || tradePartner == player) {
		player->sendCancelMessage("Select a player to trade with.");
		return;
	}

	if (!Position::areInRange<2, 2, 0>(tradePartner->getPosition(), player->getPosition())) {
		player->sendCancelMessage(RETURNVALUE_DESTINATIONOUTOFREACH);
		return;
	}

	if (!canThrowObjectTo(tradePartner->getPosition(), player->getPosition(), true, true)) {
		player->sendCancelMessage(RETURNVALUE_CANNOTTHROW);
		return;
	}

	const auto tradeThing = internalGetThing(player, pos, stackPos, 0, STACKPOS_TOPDOWN_ITEM);
	if (!tradeThing) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	auto tradeItem = tradeThing->getItem();
	if (tradeItem->getClientID() != spriteId || !tradeItem->isPickupable() || tradeItem->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID)) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	if (g_config.getBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS)) {
		if (tradeItem->getTile()->isHouseTile()) {
			if (!tradeItem->getTopParent()->getCreature() && !tradeItem->getTile()->getHouse()->isInvited(player)) {
				player->sendCancelMessage(RETURNVALUE_PLAYERISNOTINVITED);
				return;
			}
		}
	}

	const Position& playerPosition = player->getPosition();
	const Position& tradeItemPosition = tradeItem->getPosition();
	if (playerPosition.z != tradeItemPosition.z) {
		player->sendCancelMessage(playerPosition.z > tradeItemPosition.z ? RETURNVALUE_FIRSTGOUPSTAIRS : RETURNVALUE_FIRSTGODOWNSTAIRS);
		return;
	}

	if (!Position::areInRange<1, 1>(tradeItemPosition, playerPosition)) {
		if (std::vector<Direction> listDir; player->getPathTo(pos, listDir, 0, 1, true, true)) {
			g_dispatcher.addTask(createTask([this, playerID = player->getID(), listDir = std::move(listDir)]() { playerAutoWalk(playerID, listDir); }));
			const auto task = createSchedulerTask(RANGE_REQUEST_TRADE_INTERVAL, [=, this]() {
				playerRequestTrade(playerId, pos, stackPos, tradePlayerId, spriteId);
				});
			player->setNextWalkActionTask(task);
		} else {
			player->sendCancelMessage(RETURNVALUE_THEREISNOWAY);
		}
		return;
	}

	if (const auto tradeItemContainer = tradeItem->getContainer()) {
		for (const auto& key : tradeItems | std::views::keys) {
			const auto item = key;
			if (tradeItem == item) {
				player->sendCancelMessage("This item is already being traded.");
				return;
			}

			if (tradeItemContainer->isHoldingItem(item)) {
				player->sendCancelMessage("This item is already being traded.");
				return;
			}

			if (const auto container = item->getContainer(); container && container->isHoldingItem(tradeItem)) {
				player->sendCancelMessage("This item is already being traded.");
				return;
			}
		}
	} else {
		for (const auto& key : tradeItems | std::views::keys) {
			const auto item = key;
			if (tradeItem == item) {
				player->sendCancelMessage("This item is already being traded.");
				return;
			}

			if (const auto container = item->getContainer(); container && container->isHoldingItem(tradeItem)) {
				player->sendCancelMessage("This item is already being traded.");
				return;
			}
		}
	}

	if (const auto tradeContainer = tradeItem->getContainer(); tradeContainer && tradeContainer->getItemHoldingCount() + 1 > 100) {
		player->sendCancelMessage("You can only trade up to 100 objects at once.");
		return;
	}

	if (!g_events->eventPlayerOnTradeRequest(player, tradePartner, tradeItem)) {
		return;
	}

	internalStartTrade(player, tradePartner, tradeItem);
}

bool Game::internalStartTrade(const PlayerPtr& player, const PlayerPtr& tradePartner, const ItemPtr& tradeItem)
{
	if (player->tradeState != TRADE_NONE && !(player->tradeState == TRADE_ACKNOWLEDGE && player->tradePartner == tradePartner)) {
		player->sendCancelMessage(RETURNVALUE_YOUAREALREADYTRADING);
		return false;
	} else if (tradePartner->tradeState != TRADE_NONE && tradePartner->tradePartner != player) {
		player->sendCancelMessage(RETURNVALUE_THISPLAYERISALREADYTRADING);
		return false;
	}

	player->tradePartner = tradePartner;
	player->tradeItem = tradeItem;
	player->tradeState = TRADE_INITIATED;
	tradeItems[tradeItem] = player->getID();

	player->sendTradeItemRequest(player->getName(), tradeItem, true);

	if (tradePartner->tradeState == TRADE_NONE) {
		tradePartner->sendTextMessage(MESSAGE_EVENT_ADVANCE, fmt::format("{:s} wants to trade with you.", player->getName()));
		tradePartner->tradeState = TRADE_ACKNOWLEDGE;
		tradePartner->tradePartner = player;
	} else {
		const auto counterOfferItem = tradePartner->tradeItem;
		player->sendTradeItemRequest(tradePartner->getName(), counterOfferItem, false);
		tradePartner->sendTradeItemRequest(player->getName(), tradeItem, false);
	}

	return true;
}

void Game::playerAcceptTrade(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!(player->getTradeState() == TRADE_ACKNOWLEDGE || player->getTradeState() == TRADE_INITIATED)) {
		return;
	}

	const auto tradePartner = player->tradePartner;
	if (!tradePartner) {
		return;
	}

	player->setTradeState(TRADE_ACCEPT);

	if (tradePartner->getTradeState() == TRADE_ACCEPT) {
		if (!canThrowObjectTo(tradePartner->getPosition(), player->getPosition(), true, true)) {
			internalCloseTrade(player, false);
			player->sendCancelMessage(RETURNVALUE_CANNOTTHROW);
			tradePartner->sendCancelMessage(RETURNVALUE_CANNOTTHROW);
			return;
		}

		const auto playerTradeItem = player->tradeItem;
		const auto partnerTradeItem = tradePartner->tradeItem;

		if (!g_events->eventPlayerOnTradeAccept(player, tradePartner, playerTradeItem, partnerTradeItem)) {
			internalCloseTrade(player, false);
			return;
		}

		player->setTradeState(TRADE_TRANSFER);
		tradePartner->setTradeState(TRADE_TRANSFER);

		auto it = tradeItems.find(playerTradeItem);
		if (it != tradeItems.end()) {
			// ReleaseItem(it->first);
			tradeItems.erase(it);
		}

		it = tradeItems.find(partnerTradeItem);
		if (it != tradeItems.end()) {
			// ReleaseItem(it->first);
			tradeItems.erase(it);
		}

		bool isSuccess = false;

		ReturnValue tradePartnerRet = RETURNVALUE_NOERROR;
		ReturnValue playerRet = RETURNVALUE_NOERROR;

		// if player is trying to trade its own backpack
		if (tradePartner->getInventoryItem(CONST_SLOT_BACKPACK) == partnerTradeItem) {
			tradePartnerRet = (tradePartner->getInventoryItem(getSlotType(Item::items[playerTradeItem->getID()])) ? RETURNVALUE_NOTENOUGHROOM : RETURNVALUE_NOERROR);
		}

		if (player->getInventoryItem(CONST_SLOT_BACKPACK) == playerTradeItem) {
			playerRet = (player->getInventoryItem(getSlotType(Item::items[partnerTradeItem->getID()])) ? RETURNVALUE_NOTENOUGHROOM : RETURNVALUE_NOERROR);
		}

		// both players try to trade equipped backpacks
		if (player->getInventoryItem(CONST_SLOT_BACKPACK) == playerTradeItem && tradePartner->getInventoryItem(CONST_SLOT_BACKPACK) == partnerTradeItem) {
			playerRet = RETURNVALUE_NOTENOUGHROOM;
		}
		
		if (tradePartnerRet == RETURNVALUE_NOERROR && playerRet == RETURNVALUE_NOERROR) {
			CylinderPtr t_player = tradePartner;
			tradePartnerRet = internalAddItem(t_player, playerTradeItem, INDEX_WHEREEVER, 0, true);
			CylinderPtr c_player = player;
			playerRet = internalAddItem(c_player, partnerTradeItem, INDEX_WHEREEVER, 0, true);
			if (tradePartnerRet == RETURNVALUE_NOERROR && playerRet == RETURNVALUE_NOERROR) {
				playerRet = internalRemoveItem(playerTradeItem, playerTradeItem->getItemCount(), true);
				tradePartnerRet = internalRemoveItem(partnerTradeItem, partnerTradeItem->getItemCount(), true);
				if (tradePartnerRet == RETURNVALUE_NOERROR && playerRet == RETURNVALUE_NOERROR) {
					CylinderPtr t_parent = playerTradeItem->getParent();
					CylinderPtr c_parent = tradePartner;
					tradePartnerRet = internalMoveItem(t_parent, c_parent, INDEX_WHEREEVER, playerTradeItem, playerTradeItem->getItemCount(), std::nullopt, FLAG_IGNOREAUTOSTACK, nullptr, partnerTradeItem);
					if (tradePartnerRet == RETURNVALUE_NOERROR) {
						CylinderPtr pt_parent = partnerTradeItem->getParent();
						internalMoveItem(pt_parent, c_player, INDEX_WHEREEVER, partnerTradeItem, partnerTradeItem->getItemCount(), std::nullopt, FLAG_IGNOREAUTOSTACK);
						playerTradeItem->onTradeEvent(ON_TRADE_TRANSFER, tradePartner);
						partnerTradeItem->onTradeEvent(ON_TRADE_TRANSFER, player);
						isSuccess = true;
					}
				}
			}
		}

		if (!isSuccess) {
			std::string errorDescription;

			if (tradePartner->tradeItem) {
				errorDescription = getTradeErrorDescription(tradePartnerRet, playerTradeItem);
				tradePartner->sendTextMessage(MESSAGE_EVENT_ADVANCE, errorDescription);
				tradePartner->tradeItem->onTradeEvent(ON_TRADE_CANCEL, tradePartner);
			}

			if (player->tradeItem) {
				errorDescription = getTradeErrorDescription(playerRet, partnerTradeItem);
				player->sendTextMessage(MESSAGE_EVENT_ADVANCE, errorDescription);
				player->tradeItem->onTradeEvent(ON_TRADE_CANCEL, player);
			}
		}

		g_events->eventPlayerOnTradeCompleted(player, tradePartner, playerTradeItem, partnerTradeItem, isSuccess);

		player->setTradeState(TRADE_NONE);
		player->tradeItem = nullptr;
		player->tradePartner = nullptr;
		player->sendTradeClose();

		tradePartner->setTradeState(TRADE_NONE);
		tradePartner->tradeItem = nullptr;
		tradePartner->tradePartner = nullptr;
		tradePartner->sendTradeClose();
	}
}

std::string Game::getTradeErrorDescription(const ReturnValue ret, const ItemPtr& item)
{
	if (item) {
		if (ret == RETURNVALUE_NOTENOUGHCAPACITY) {
			return fmt::format("You do not have enough capacity to carry {:s}.\n {:s}", item->isStackable() && item->getItemCount() > 1 ? "these objects" : "this object", item->getWeightDescription());
		} else if (ret == RETURNVALUE_NOTENOUGHROOM || ret == RETURNVALUE_CONTAINERNOTENOUGHROOM) {
			return fmt::format("You do not have enough room to carry {:s}.", item->isStackable() && item->getItemCount() > 1 ? "these objects" : "this object");
		}
	}
	return "Trade could not be completed.";
}

void Game::playerLookInTrade(const uint32_t playerId, const bool lookAtCounterOffer, uint8_t index)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto tradePartner = player->tradePartner;
	if (!tradePartner) {
		return;
	}

	ItemPtr tradeItem;
	if (lookAtCounterOffer) {
		tradeItem = tradePartner->getTradeItem();
	} else {
		tradeItem = player->getTradeItem();
	}

	if (!tradeItem) {
		return;
	}

	const Position& playerPosition = player->getPosition();
	const Position& tradeItemPosition = tradeItem->getPosition();

	int32_t lookDistance = std::max<int32_t>(Position::getDistanceX(playerPosition, tradeItemPosition),
	                                         Position::getDistanceY(playerPosition, tradeItemPosition));
	if (index == 0) {
		g_events->eventPlayerOnLookInTrade(player, tradePartner, tradeItem, lookDistance);
		return;
	}

	const auto tradeContainer = tradeItem->getContainer();
	if (!tradeContainer) {
		return;
	}

	std::vector<ContainerPtr> containers {tradeContainer};
	size_t i = 0;
	while (i < containers.size()) {
		for (const auto container = containers[i++]; const auto item : container->getItemList()) {
			if (auto tmpContainer = item->getContainer()) {
				containers.push_back(tmpContainer);
			}

			if (--index == 0) {
				g_events->eventPlayerOnLookInTrade(player, tradePartner, item, lookDistance);
				return;
			}
		}
	}
}

void Game::playerCloseTrade(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	internalCloseTrade(player);
}

void Game::internalCloseTrade(const PlayerPtr& player, const bool sendCancel/* = true*/)
{
	auto tradePartner = player->tradePartner;
	if ((tradePartner && tradePartner->getTradeState() == TRADE_TRANSFER) || player->getTradeState() == TRADE_TRANSFER) {
		return;
	}

	if (player->getTradeItem()) {
		if (const auto it = tradeItems.find(player->getTradeItem()); it != tradeItems.end()) {
			// ReleaseItem(it->first);
			tradeItems.erase(it);
		}

		player->tradeItem->onTradeEvent(ON_TRADE_CANCEL, player);
		player->tradeItem = nullptr;
	}

	player->setTradeState(TRADE_NONE);
	player->tradePartner = nullptr;

	if (sendCancel) {
		player->sendTextMessage(MESSAGE_STATUS_SMALL, "Trade cancelled.");
	}
	player->sendTradeClose();

	if (tradePartner) {
		if (tradePartner->getTradeItem()) {
			if (const auto it = tradeItems.find(tradePartner->getTradeItem()); it != tradeItems.end()) {
				// ReleaseItem(it->first);
				tradeItems.erase(it);
			}

			tradePartner->tradeItem->onTradeEvent(ON_TRADE_CANCEL, tradePartner);
			tradePartner->tradeItem = nullptr;
		}

		tradePartner->setTradeState(TRADE_NONE);
		tradePartner->tradePartner = nullptr;

		if (sendCancel) {
			tradePartner->sendTextMessage(MESSAGE_STATUS_SMALL, "Trade cancelled.");
		}
		tradePartner->sendTradeClose();
	}
}

void Game::playerPurchaseItem(const uint32_t playerId, const uint16_t spriteId, const uint8_t count, uint8_t amount,
                              bool ignoreCap/* = false*/, bool inBackpacks/* = false*/)
{
	if (amount == 0 || amount > 100) {
		return;
	}

	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	int32_t onBuy, onSell;

	auto merchant = player->getShopOwner(onBuy, onSell);
	if (!merchant) {
		return;
	}

	const ItemType& it = Item::items.getItemIdByClientId(spriteId);
	if (it.id == 0) {
		return;
	}

	uint8_t subType;
	if (it.isSplash() || it.isFluidContainer()) {
		subType = clientFluidToServer(count);
	} else {
		subType = count;
	}

	if (!player->hasShopItemForSale(it.id, subType)) {
		return;
	}

	merchant->onPlayerTrade(player, onBuy, it.id, subType, amount, ignoreCap, inBackpacks);
}

void Game::playerSellItem(const uint32_t playerId, const uint16_t spriteId, const uint8_t count, uint8_t amount, bool ignoreEquipped)
{
	if (amount == 0 || amount > 100) {
		return;
	}

	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	int32_t onBuy, onSell;

	auto merchant = player->getShopOwner(onBuy, onSell);
	if (!merchant) {
		return;
	}

	const ItemType& it = Item::items.getItemIdByClientId(spriteId);
	if (it.id == 0) {
		return;
	}

	uint8_t subType;
	if (it.isSplash() || it.isFluidContainer()) {
		subType = clientFluidToServer(count);
	} else {
		subType = count;
	}

	merchant->onPlayerTrade(player, onSell, it.id, subType, amount, ignoreEquipped);
}

void Game::playerCloseShop(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->closeShopWindow();
}

void Game::playerLookInShop(const uint32_t playerId, const uint16_t spriteId, const uint8_t count)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	int32_t onBuy, onSell;

	if (const auto merchant = player->getShopOwner(onBuy, onSell); !merchant) {
		return;
	}

	const ItemType& it = Item::items.getItemIdByClientId(spriteId);
	if (it.id == 0) {
		return;
	}

	int32_t subType;
	if (it.isFluidContainer() || it.isSplash()) {
		subType = clientFluidToServer(count);
	} else {
		subType = count;
	}

	if (!player->hasShopItemForSale(it.id, subType)) {
		return;
	}

	const std::string& description = Item::getDescription(it, 1, nullptr, subType);
	g_events->eventPlayerOnLookInShop(player, &it, subType, description);
}

void Game::playerLookAt(const uint32_t playerId, const Position& pos, uint8_t stackPos)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto thing = internalGetThing(player, pos, stackPos, 0, STACKPOS_LOOK);
	if (!thing) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	const Position thingPos = thing->getPosition();
	if (!player->canSee(thingPos)) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return;
	}

	const Position playerPos = player->getPosition();

	int32_t lookDistance;
	if (thing != player) {
		lookDistance = std::max<int32_t>(Position::getDistanceX(playerPos, thingPos), Position::getDistanceY(playerPos, thingPos));
		if (playerPos.z != thingPos.z) {
			lookDistance += 15;
		}
	} else {
		lookDistance = -1;
	}

	g_events->eventPlayerOnLook(player, pos, thing, stackPos, lookDistance);
}

void Game::playerLookInBattleList(const uint32_t playerId, const uint32_t creatureId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto creature = getCreatureByID(creatureId);
	if (!creature) {
		return;
	}

	if (!player->canSeeCreature(creature)) {
		return;
	}

	const Position& creaturePos = creature->getPosition();
	if (!player->canSee(creaturePos)) {
		return;
	}

	int32_t lookDistance;
	if (creature != player) {
		const Position& playerPos = player->getPosition();
		lookDistance = std::max<int32_t>(Position::getDistanceX(playerPos, creaturePos), Position::getDistanceY(playerPos, creaturePos));
		if (playerPos.z != creaturePos.z) {
			lookDistance += 15;
		}
	} else {
		lookDistance = -1;
	}

	g_events->eventPlayerOnLookInBattleList(player, creature, lookDistance);
}

void Game::playerCancelAttackAndFollow(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	playerSetAttackedCreature(playerId, 0);
	playerFollowCreature(playerId, 0);
	player->stopWalk();
}

void Game::playerSetAttackedCreature(const uint32_t playerId, const uint32_t creatureId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (player->getAttackedCreature() && creatureId == 0) {
		player->setAttackedCreature(nullptr);
		player->sendCancelTarget();
		return;
	}

	const auto attackCreature = getCreatureByID(creatureId);
	if (!attackCreature) {
		player->setAttackedCreature(nullptr);
		player->sendCancelTarget();
		return;
	}

	const ReturnValue ret = Combat::canTargetCreature(player, attackCreature);
	if (ret != RETURNVALUE_NOERROR) {
		player->sendCancelMessage(ret);
		player->sendCancelTarget();
		player->setAttackedCreature(nullptr);
		return;
	}

	player->setAttackedCreature(attackCreature);
	g_dispatcher.addTask(createTask([this, id = player->getID()]() { updateCreatureWalk(id); }));
}

void Game::playerFollowCreature(const uint32_t playerId, const uint32_t creatureId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->setAttackedCreature(nullptr);
	g_dispatcher.addTask(createTask([this, id = player->getID()]() { updateCreatureWalk(id); }));
	player->setFollowCreature(getCreatureByID(creatureId));
}

void Game::playerSetFightModes(const uint32_t playerId, const fightMode_t fightMode, const bool chaseMode, const bool secureMode)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->setFightMode(fightMode);
	player->setChaseMode(chaseMode);
	player->setSecureMode(secureMode);
}

void Game::playerRequestAddVip(const uint32_t playerId, const std::string& name)
{
	if (name.length() > PLAYER_NAME_LENGTH) {
		return;
	}

	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto vipPlayer = getPlayerByName(name);
	if (!vipPlayer) {
		uint32_t guid;
		bool specialVip;
		std::string formattedName = name;
		if (!IOLoginData::getGuidByNameEx(guid, specialVip, formattedName)) {
			player->sendTextMessage(MESSAGE_STATUS_SMALL, "A player with this name does not exist.");
			return;
		}

		if (specialVip && !player->hasFlag(PlayerFlag_SpecialVIP)) {
			player->sendTextMessage(MESSAGE_STATUS_SMALL, "You can not add this player.");
			return;
		}

		player->addVIP(guid, formattedName, VIPSTATUS_OFFLINE);
	} else {
		if (vipPlayer->hasFlag(PlayerFlag_SpecialVIP) && !player->hasFlag(PlayerFlag_SpecialVIP)) {
			player->sendTextMessage(MESSAGE_STATUS_SMALL, "You can not add this player.");
			return;
		}

		if (!vipPlayer->isInGhostMode() || player->canSeeGhostMode(vipPlayer)) {
			player->addVIP(vipPlayer->getGUID(), vipPlayer->getName(), VIPSTATUS_ONLINE);
		} else {
			player->addVIP(vipPlayer->getGUID(), vipPlayer->getName(), VIPSTATUS_OFFLINE);
		}
	}
}

void Game::playerRequestRemoveVip(const uint32_t playerId, const uint32_t guid)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->removeVIP(guid);
}

void Game::playerRequestEditVip(const uint32_t playerId, const uint32_t guid, const std::string& description, const uint32_t icon, const bool notify)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->editVIP(guid, description, icon, notify);
}

void Game::playerTurn(const uint32_t playerId, const Direction dir)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!g_events->eventPlayerOnTurn(player, dir)) {
		return;
	}

	player->resetIdleTime();
	internalCreatureTurn(player, dir);
}

void Game::playerRequestOutfit(const uint32_t playerId)
{
	if (!g_config.getBoolean(ConfigManager::ALLOW_CHANGEOUTFIT)) {
		return;
	}

	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->sendOutfitWindow();
}

void Game::playerToggleMount(const uint32_t playerId, const bool mount)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->toggleMount(mount);
}

void Game::playerChangeOutfit(const uint32_t playerId, Outfit_t outfit)
{
	if (!g_config.getBoolean(ConfigManager::ALLOW_CHANGEOUTFIT)) {
		return;
	}

	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (const auto playerOutfit = Outfits::getInstance().getOutfitByLookType(player->getSex(), outfit.lookType); !playerOutfit) {
		outfit.lookMount = 0;
	}

	if (outfit.lookMount != 0) {
		const auto mount = mounts.getMountByClientID(outfit.lookMount);
		if (!mount) {
			return;
		}

		if (!player->hasMount(mount)) {
			return;
		}

		if (player->isMounted()) {
			if (const auto prevMount = mounts.getMountByID(player->getCurrentMount())) {
				changeSpeed(player, mount->speed - prevMount->speed);
			}

			player->setCurrentMount(mount->id);
		} else {
			player->setCurrentMount(mount->id);
			outfit.lookMount = 0;
		}
	} else if (player->isMounted()) {
		player->dismount();
	}

	if (player->canWear(outfit.lookType, outfit.lookAddons)) {
		player->defaultOutfit = outfit;

		if (player->hasCondition(CONDITION_OUTFIT)) {
			return;
		}

		internalCreatureChangeOutfit(player, outfit);
	}
}

void Game::playerShowQuestLog(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->sendQuestLog();
}

void Game::playerShowQuestLine(const uint32_t playerId, const uint16_t questId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto quest = quests.getQuestByID(questId);
	if (!quest) {
		return;
	}

	player->sendQuestLine(quest);
}

void Game::playerSay(const uint32_t playerId, const uint16_t channelId, const SpeakClasses type,
                     const std::string& receiver, const std::string& text)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->resetIdleTime();

	if (playerSaySpell(player, type, text)) {
		return;
	}

	if (type == TALKTYPE_PRIVATE_PN) {
		playerSpeakToNpc(player, text);
		return;
	}

	if (const uint32_t muteTime = player->isMuted(); muteTime > 0) {
		player->sendTextMessage(MESSAGE_STATUS_SMALL, fmt::format("You are still muted for {:d} seconds.", muteTime));
		return;
	}

	if (!text.empty() && text.front() == '/' && player->isAccessPlayer()) {
		return;
	}

	player->removeMessageBuffer();

	switch (type) {
		case TALKTYPE_SAY:
			internalCreatureSay(player, TALKTYPE_SAY, text, false);
			break;

		case TALKTYPE_WHISPER:
			playerWhisper(player, text);
			break;

		case TALKTYPE_YELL:
			playerYell(player, text);
			break;

		case TALKTYPE_PRIVATE_TO:
		case TALKTYPE_PRIVATE_RED_TO:
			playerSpeakTo(player, type, receiver, text);
			break;

		case TALKTYPE_CHANNEL_O:
		case TALKTYPE_CHANNEL_Y:
		case TALKTYPE_CHANNEL_R1:
			g_chat->talkToChannel(player, type, text, channelId);
			break;

		case TALKTYPE_BROADCAST:
			playerBroadcastMessage(player, text);
			break;

		default:
			break;
	}
}

ModalWindow Game::CreatePrivateAccountManagerWindow(const uint32_t modalWindowId, const uint32_t optionId /* = 0 */)
{	// todo : trade out magic numbers here and below with enums
	auto window = ModalWindow(modalWindowId, "Account Manager", "");
	window.priority = true;
	window.defaultEnterButton = ButtonID::PRIMARY;
	window.defaultEscapeButton = ButtonID::SECONDARY;


	switch (modalWindowId)
	{
		// MAIN MENU
		case AccountManager::PRIVATE_LOGIN:
		{
			window.message = "Welcome back my friend! How may I be of assistance today?";
			window.choices.emplace_back("Create Character", ChoiceID::FIRST); // enums
			window.choices.emplace_back("Create New Account", ChoiceID::SECOND);
			window.choices.emplace_back("Change Password", ChoiceID::THIRD);
			window.buttons.emplace_back("Select", ButtonID::PRIMARY);
			window.buttons.emplace_back("Quit", ButtonID::SECONDARY);
			break;

		}

		// CHARACTER CREATION
		case AccountManager::PRIVATE_CHARACTER_VOCATION: // asks for vocation
		{
			// Todo : filter by account premium status or not
			window.message = "This is a great day to be born anew! Which cast shall you take?";
			for (const auto& choice : character_options) 
			{
				auto sex = choice.sex ? " (male)\n" : " (female)\n";
				auto displayName = choice.name + sex;
				window.choices.emplace_back(displayName, choice.id);
			}
			window.buttons.emplace_back("Select", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_CHARACTER_TOWN: // asks for town
		{
			window.message = "Ahhh, yes. A good fit for ye indeed!\n\nNow choose where to begin:";

			for (const auto& town_id : character_options[optionId].town_list)
			{
				const auto& town = map.towns.getTown(town_id);
				window.choices.emplace_back(town->getName(), town_id);
			}

			window.buttons.emplace_back("Select", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			window.buttons.emplace_back("Cancel", ButtonID::TERTIARY);
			break;
		}

		case AccountManager::PRIVATE_CHARACTER: // asks for name
		{
			window.message = "Now, what shall we call you?\n\nYou ponder that and let me know when you are ready!";
			window.buttons.emplace_back("Ready", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			window.buttons.emplace_back("Main Menu", ButtonID::TERTIARY);
			break;
		}

		case AccountManager::PRIVATE_CHARACTER_FAILED: // asks for name
		{
			window.title = "Unacceptable Name";
			window.message = "Well I know you like to be creative, but we both know you can't use that as a character name... I dopn't believe I need to tell you why!\n\n Let me know when you have come up with something more appropriate";
			window.buttons.emplace_back("Ready", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			window.buttons.emplace_back("Main Menu", ButtonID::TERTIARY);
			break;
		}

		case AccountManager::PRIVATE_CHARACTER_CONFIRMATION:
		{
			window.message = "Well then, not exactly what I would have taken, but hey if you are certain..\n\nYou are certain, right?";
			window.buttons.emplace_back("Yes", ButtonID::PRIMARY);
			window.buttons.emplace_back("No", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_CHARACTER_SUCCESS: // says you successfully created a character
		{
			window.message = "\nBy my Divine Power, It is done!\n";
			window.buttons.emplace_back("Main Menu", ButtonID::PRIMARY);
			window.buttons.emplace_back("Logout", ButtonID::SECONDARY);
			break;
		}

		// ACCOUNT CREATION
		case AccountManager::PRIVATE_ACCOUNT:
		{
			window.message = "I know that you have some experience with this already, but just as a reminder...\n\nAn account name or password must abide by the following rules:\n\n- Must be a minimum of 6 characters and maximum of 15\n- Must contain no special characters or symbols\n- Can contain no space\n\n Do you understand and wish to continue?";
			window.buttons.emplace_back("Yes", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_ACCOUNT_FAILED:
		{
			window.message = "The account name you entered was not accepted for one of the following reasons:\n\n-It contained illegal characters, such as symbols or spaces\n-It contained more characters than allowed\n-It did not contain enough characters\n-Account Name is forbidden or in use already.\n\nAre you ready to try again?";
			window.buttons.emplace_back("Retry", ButtonID::PRIMARY);
			window.buttons.emplace_back("Main Menu", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_WINDOW: // tells you password requirements
		{
			window.message = "Passwords must adhere to the following requirements\n\nCannot:\n\n-Contain symbols\n-Contain spaces\n-Contain more than 15 characters\n-Contain less than 6 characters\n\nDo you understand the requirements and wish to proceed?";
			window.buttons.emplace_back("Yes", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_CONFIRMATION: // tells you to confirm password
		{
			window.message = "Well chosen! I just need you to confirm that password one more time for me, ok?";
			window.buttons.emplace_back("Yes", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			window.buttons.emplace_back("Cancel", ButtonID::TERTIARY);
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_SUCCESS: // tells you password was successfully changed
		{
			window.title = "Success!";
			window.message = "Congratulations! You have successfully created a new account, you may login with it anytime to create a character!\n\n What would you like to do?";
			window.buttons.emplace_back("Logout", ButtonID::PRIMARY);
			window.buttons.emplace_back("Main Menu", ButtonID::SECONDARY);
			break;
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_FAILED: // says password doesn't meet requirements
		{
			window.message = "The password you have entered was not accepted because of one of the following reasons:\n\n-Contains symbols\n-Contains spaces\n-Contains more than 15 characters\n-Contains less than 6 characters\n\nAre you ready to try a different password?";
			window.buttons.emplace_back("Yes", ButtonID::PRIMARY);
			window.buttons.emplace_back("Cancel", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_MISMATCH: // says passwords don't match
		{
			window.title = "Invalid Password!";
			window.message = "I'm sorry, but that didn't quite match what you entered the first time. Please ensure you enter the same password you entered before..\nAre you ready to proceed? Or shall we start again?";
			window.buttons.emplace_back("Retry", ButtonID::PRIMARY);
			window.buttons.emplace_back("New Pass", ButtonID::SECONDARY);
			window.buttons.emplace_back("Main Menu", ButtonID::TERTIARY);
			break;
		}

		// PASSWORD RESET
		case AccountManager::PRIVATE_PASSWORD_RESET:
		{
			window.message = "Passwords must adhere to the following requirements\n\nCannot:\n\n-Contain symbols\n-Contain spaces\n-Contain more than 15 characters\n-Contain less than 6 characters\n\nDo you understand the requirements and wish to proceed?";
			window.buttons.emplace_back("Yes", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_RESET_CONFIRMATION: // tells you to confirm password
		{
			window.message = "Well chosen! I just need you to confirm that password one more time for me, ok?";
			window.buttons.emplace_back("Yes", ButtonID::PRIMARY);
			window.buttons.emplace_back("Back", ButtonID::SECONDARY);
			window.buttons.emplace_back("Cancel", ButtonID::TERTIARY);
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_RESET_SUCCESS: // tells you password was successfully changed
		{
			window.title = "Success!";
			window.message = "Excellent job, wise master! Your password has been successfully changed!\n\nWhat would you like to do now?";
			window.buttons.emplace_back("Main Menu", ButtonID::PRIMARY);
			window.buttons.emplace_back("Logout", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_RESET_FAILED: // says password doesn't meet requirements
		{
			window.title = "Failed!";
			window.message = "The password you have entered was not accepted because of one of the following reasons:\n\n-Contains symbols\n-Contains spaces\n-Contains more than 15 characters\n-Contains less than 6 characters\n\nAre you ready to try a different password?";
			window.buttons.emplace_back("Yes", ButtonID::PRIMARY);
			window.buttons.emplace_back("Cancel", ButtonID::SECONDARY);
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_RESET_MISMATCH: // says passwords don't match
		{
			window.title = "Password Mismatch!";
			window.message = "I'm sorry, but that didn't quite match what you entered the first time. Please ensure you enter the same password you entered before..\nAre you ready to proceed? Or shall we start again?";
			window.buttons.emplace_back("Retry", ButtonID::PRIMARY);
			window.buttons.emplace_back("New Pass", ButtonID::SECONDARY);
			window.buttons.emplace_back("Main Menu", ButtonID::TERTIARY);
			break;
		}
	}
	return window;
}

void Game::onPrivateAccountManagerInput(const PlayerPtr& player, const uint32_t modalWindowId, const uint8_t button, const uint8_t choice)
{
	switch (modalWindowId)
	{
		/////////////////////////////////
		//////// MAIN MENU
		/////////////////////////////////
		case AccountManager::PRIVATE_LOGIN:
		{
			if (button == ButtonID::PRIMARY)
			{
				if (choice == ChoiceID::FIRST)
				{
					player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_VOCATION));
					return;
				}
				if (choice == ChoiceID::SECOND)
				{
					player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_ACCOUNT));
					return;
				}
				if (choice == ChoiceID::THIRD)
				{
					player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_RESET));
					return;
				}
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->kickPlayer(false);
				return;
			}
			break;
		}

		/////////////////////////////////
		//////// CHARACTER CREATION
		/////////////////////////////////
		case AccountManager::PRIVATE_CHARACTER_VOCATION:
		{
			if (button == ButtonID::PRIMARY)
			{	
				player->setTempCharacterChoice(choice);
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_TOWN, choice));
				return;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				return;
			}
			break;
		}

		case AccountManager::PRIVATE_CHARACTER_TOWN:
		{	// this window should only be sent if the choice has towns and there is more than 1.
			if (button == ButtonID::PRIMARY)
			{
				const auto& town = map.towns.getTown(choice);
				const auto& spawn_pos = town->getTemplePosition();
				player->setTempPosition(spawn_pos);
				player->setTempTownId(choice);
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER));
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->onModalWindowHandled(modalWindowId);
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_VOCATION));
				break;
			}
			if (button == ButtonID::TERTIARY)
			{
				player->onModalWindowHandled(modalWindowId);
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_CHARACTER:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->sendAccountManagerTextWindow(AccountManager::CHARACTER_NAME_TEXT_BOX, "Character Name");
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_TOWN, player->getTempVocation()));
				break;
			}
			if (button == ButtonID::TERTIARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_CHARACTER_FAILED:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->sendAccountManagerTextWindow(AccountManager::CHARACTER_NAME_TEXT_BOX, "Character Name");
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_TOWN, player->getTempCharacterChoice()));
				break;
			}
			if (button == ButtonID::TERTIARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_CHARACTER_SUCCESS:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->kickPlayer(false);
				break;
			}
			break;
		}

		/////////////////////////////////
		//////// NEW ACCOUNT CREATION
		/////////////////////////////////
		case AccountManager::PRIVATE_ACCOUNT:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->sendAccountManagerTextWindow(AccountManager::ACCOUNT_NAME_TEXT_BOX, "Account Name");
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_ACCOUNT_FAILED:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->sendAccountManagerTextWindow(AccountManager::ACCOUNT_NAME_TEXT_BOX, "Account Name");
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_WINDOW:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->sendAccountManagerTextWindow(AccountManager::PASSWORD_TEXT_BOX, "Password");
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_CONFIRMATION:
		{
			if (button == ButtonID::PRIMARY) // Yes
			{
				player->sendAccountManagerTextWindow(AccountManager::CONFIRMATION_TEXT_BOX, "Repeat Password");
				break;
			}
			if (button == ButtonID::SECONDARY) // Back
			{
				player->sendAccountManagerTextWindow(AccountManager::PASSWORD_TEXT_BOX, "Password");
				break;
			}
			if (button == ButtonID::TERTIARY) // Cancel
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_SUCCESS:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->kickPlayer(false);
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_FAILED:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->sendAccountManagerTextWindow(AccountManager::CONFIRMATION_TEXT_BOX, "Repeat Password");
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;

		}

		case AccountManager::PRIVATE_PASSWORD_MISMATCH:
		{

			if (button == ButtonID::PRIMARY) // Retry
			{
				player->sendAccountManagerTextWindow(AccountManager::CONFIRMATION_TEXT_BOX, "Repeat Password");
				break;
			}
			if (button == ButtonID::SECONDARY) // New pass
			{
				player->setTempPassword("");
				player->sendAccountManagerTextWindow(AccountManager::CHARACTER_NAME_TEXT_BOX, "Character Name");
				break;
			}
			if (button == ButtonID::TERTIARY) // Main Menu
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		/////////////////////////////////
		//////// PASSWORD RESET
		/////////////////////////////////
		case AccountManager::PRIVATE_PASSWORD_RESET:
		{
			if (button == ButtonID::PRIMARY) // Yes
			{
				player->sendAccountManagerTextWindow(AccountManager::PASSWORD_RESET, "New Password");
				break;
			}
			if (button == ButtonID::SECONDARY) // No / Cancel / Back
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_RESET_CONFIRMATION:
		{
			if (button == ButtonID::PRIMARY) // Yes
			{
				player->sendAccountManagerTextWindow(AccountManager::PASSWORD_RESET_CONFIRM, "Repeat New Password");
				break;
			}
			if (button == ButtonID::SECONDARY) // Back
			{
				player->sendAccountManagerTextWindow(AccountManager::PASSWORD_RESET, "New Password");
				break;
			}
			if (button == ButtonID::TERTIARY) // Cancel
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_RESET_SUCCESS:
		{
			if (button == ButtonID::PRIMARY)
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			if (button == ButtonID::SECONDARY)
			{
				player->kickPlayer(true);
				break;
			}
			break;
		}

		case AccountManager::PRIVATE_PASSWORD_RESET_FAILED:
		{
			if (button == ButtonID::PRIMARY) // Retry
			{
				player->sendAccountManagerTextWindow(AccountManager::PASSWORD_RESET_CONFIRM, "Repeat New Password");
				break;
			}
			if (button == ButtonID::SECONDARY) // Cancel
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;

		}

		case AccountManager::PRIVATE_PASSWORD_RESET_MISMATCH:
		{

			if (button == ButtonID::PRIMARY) // Retry
			{
				player->sendAccountManagerTextWindow(AccountManager::PASSWORD_RESET_CONFIRM, "Repeat New Password");
				break;
			}
			if (button == ButtonID::SECONDARY) // New Pass
			{
				player->setTempPassword("");
				player->sendAccountManagerTextWindow(AccountManager::PASSWORD_RESET, "New Password");
				break;
			}
			if (button == ButtonID::TERTIARY) // Main Menu
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN));
				break;
			}
			break;
		}
	}
	player->onModalWindowHandled(modalWindowId);
}

void Game::onPrivateAccountManagerRecieveText(const uint32_t player_id, uint32_t window_id, const std::string& text)
{
	const auto player = getPlayerByID(player_id);
	if (!player)
	{
		return;
	}
	switch (window_id)
	{
		case AccountManager::ACCOUNT_NAME_TEXT_BOX: // Received Account Name
		{
			if (isAllowedRegistration(text) and not IOLoginData::accountExists(text))
			{
				player->setTempAccountName(text);
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_WINDOW));
				break;
			} // else

			player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_ACCOUNT_FAILED));
			break;
		}

		case AccountManager::PASSWORD_TEXT_BOX: // Received Password
		{
			if (isAllowedRegistration(text))
			{
				player->setTempPassword(text);
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_CONFIRMATION));
				break;
			} // else

			player->setTempPassword("");
			player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_FAILED));
			break;
		}

		case AccountManager::CONFIRMATION_TEXT_BOX: // Recieved Confirmation Password
		{
			if (text == player->getTempPassword())
			{
				Database& db = Database::getInstance();

				db.executeQuery(fmt::format("INSERT INTO `accounts` (`name`, `password`, `secret`, `type`, `premium_ends_at`, `email`, `creation`) "
					"VALUES ({:s}, {:s}, NULL, 1, 0, '', UNIX_TIMESTAMP())",
					db.escapeString(player->getTempAccountName()),
					db.escapeString(transformToSHA1(text))));

				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_SUCCESS));
				player->setTempAccountName("");
				player->setTempPassword("");
				break;
			} // else

			player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_MISMATCH));
			break;
		}

		case AccountManager::CHARACTER_NAME_TEXT_BOX:
		{
			for (char ch : text) {
				if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != ' ') {
					// invalid name, contains symbols
					player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_FAILED));
					break;
				}
			}

			// insert here any list of names to filter, probably in config.lua or extend the accountmanager.toml parser.

			auto& db = Database::getInstance();
			const auto& config = character_options[player->getTempCharacterChoice()];
			const auto& vocation = g_vocations.getVocation(config.vocation);
			const auto& startingPos = player->getTempPosition();
			auto sex = config.sex ? 1 : 0;

			std::string query = fmt::format(fmt::runtime(
				"INSERT INTO `players` ("
				"`account_id`, `name`, `vocation`, `maglevel`, `sex`, "
				"`skill_fist`,`skill_fist_tries`,"
				"`skill_club`,`skill_club_tries`,"
				"`skill_sword`,`skill_sword_tries`,"
				"`skill_axe`,`skill_axe_tries`,"
				"`skill_dist`,`skill_dist_tries`,"
				"`skill_shielding`,`skill_shielding_tries`,"
				"`skill_fishing`,`skill_fishing_tries`,"
				"`town_id`,`posx`,`posy`,`posz`"
				") VALUES ("
				"{:d}, {:s}, {:d}, {:d}, {:d},"
				"{:d}, {:d}, "
				"{:d}, {:d}, "
				"{:d}, {:d}, "
				"{:d}, {:d}, "
				"{:d}, {:d}, "
				"{:d}, {:d}, "
				"{:d}, {:d}, "
				"{:d}, {:d}, {:d}, {:d}"
				")"),
				player->getAccount(),
				db.escapeString(text),
				config.vocation,
				config.magiclevel,
				sex,
				config.skills[SKILL_FIST], vocation->getReqSkillTries(SKILL_FIST, config.skills[SKILL_FIST]),
				config.skills[SKILL_CLUB], vocation->getReqSkillTries(SKILL_CLUB, config.skills[SKILL_CLUB]),
				config.skills[SKILL_SWORD], vocation->getReqSkillTries(SKILL_SWORD, config.skills[SKILL_SWORD]),
				config.skills[SKILL_AXE], vocation->getReqSkillTries(SKILL_AXE, config.skills[SKILL_AXE]),
				config.skills[SKILL_DISTANCE], vocation->getReqSkillTries(SKILL_DISTANCE, config.skills[SKILL_DISTANCE]),
				config.skills[SKILL_SHIELD], vocation->getReqSkillTries(SKILL_SHIELD, config.skills[SKILL_SHIELD]),
				config.skills[SKILL_FISHING], vocation->getReqSkillTries(SKILL_FISHING, config.skills[SKILL_FISHING]),
				player->getTempTownId(),
				startingPos.x, startingPos.y, startingPos.z
			);

			if (const auto& result = db.executeQuery(query))
			{
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_SUCCESS));
				break;
			} // else

			player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_FAILED));
			break;
		}
		
		case AccountManager::PASSWORD_RESET:
		{
			if (isAllowedRegistration(text))
			{
				player->setTempPassword(text);
				player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_RESET_CONFIRMATION));
				break;
			} // else

			player->setTempPassword("");
			player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_RESET_FAILED));
			break;
		}

		case AccountManager::PASSWORD_RESET_CONFIRM:
		{
			if (text == player->getTempPassword())
			{
				Database& db = Database::getInstance();
				Account account = IOLoginData::loadAccount(player->getAccount());
				if (db.executeQuery(fmt::format("UPDATE `accounts` SET `password` = {:s} WHERE `name` = {:s}",
					db.escapeString(transformToSHA1(text)), db.escapeString(account.name))))
				{
					player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_RESET_SUCCESS));
					player->setTempAccountName("");
					player->setTempPassword("");
					break;
				} // else

				// log and/or send a failure window
				break;
			}

			player->sendModalWindow(CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_PASSWORD_RESET_MISMATCH));
			break;
		}

		default:
		{
			break;
		}
	}
}

ModalWindow Game::CreateAccountManagerWindow(const uint32_t modalWindowId)
{	
	using Button = std::pair<std::string, uint8_t>;

	auto window = ModalWindow(modalWindowId, "Account Manager", "");
	window.priority = true;
	window.defaultEnterButton = 1;
	window.defaultEscapeButton = 2;
	
	Button Accept = { "Yes", 1 };
	Button Decline = { "No", 2 };
	Button Ok = { "Ok", 1 };
	Button RetryPassword = {"Retry", 1};
	Button NewPassword = { "New Pass", 2 };
	Button Restart = { "Restart", 3 };

	window.buttons.emplace_back(Accept);
	window.buttons.emplace_back(Decline);

	switch(modalWindowId)
	{
		case AccountManager::COMMON_LOGIN:
		{
			window.title = "Welcome!";
			window.message = "Welcome new spirit, are you ready to begin your destiny?";
			break;
		}

		case AccountManager::COMMON_ACCOUNT:
		{
			window.message = "First thing we need to do is create an account name. \nAn account name or password must abide by the following rules:\n\n- Must be a minimum of 6 characters and maximum of 15\n- Must contain no special characters or symbols\n- Can contain no space\n\n Do you understand and wish to continue?";
			break;
		}

		case AccountManager::COMMON_PASSWORD:
		{
			window.message = "Perfect! Now we shall set a password, ok?";
			break;
		}

		case AccountManager::COMMON_CONFIRMATION:
		{
			window.message = "Well chosen! I just need you to confirm that password one more time for me, ok?";
			break;
		}

		case AccountManager::COMMON_SUCCESS:
		{
			window.title = "Success!";
			window.message = "Your account has been successfully created! Please logout, and then back in using your newly created credentials";
			window.buttons.clear();
			window.buttons.emplace_back(Ok);
			break;
		}

		case AccountManager::COMMON_ACCOUNT_FAILED:
		{
			window.title = "Account Creation Failed!";
			window.message = "The account name you entered was not accepted for one of the following reasons:\n\n-It contained illegal characters, such as symbols or spaces\n-It contained more characters than allowed\n-It did not contain enough characters\n-Account Name is forbidden or in use already.\n\nAre you ready to try again?";
			break;
		}

		case AccountManager::COMMON_PASSWORD_FAILED:
		{
			window.title = "Invalid Password!";
			window.message = "The password you have entered was not accepted because of one of the following reasons:\n\n-Contains symbols\n-Contains spaces\n-Contains more than 15 characters\n-Contains less than 6 characters\n\nAre you ready to try a different password?";
			break;
		}

		case AccountManager::COMMON_CONFIRMATION_FAILED:
		{
			window.message = "Your passwords did not match! Please retry your chosen password or choose a new password.";
			window.buttons.clear();
			window.buttons.emplace_back(RetryPassword);
			window.buttons.emplace_back(NewPassword);
			window.buttons.emplace_back(Restart);
			break;
		}

		case AccountManager::COMMON_CANCEL:
		{
			window.title = "Quit?";
			window.message = "If you quit now, all your progress will be lost! \n\nYou can restart if that is what you wish.\nAre you certain you wish to cancel and exit the game";
			window.buttons.clear();
			window.buttons.emplace_back("Exit", 1); // enums needed here
			window.buttons.emplace_back("Back", 2);
			window.buttons.emplace_back(Restart);
			break;
		}
	}
	return window;
}

void Game::onAccountManagerRecieveText(const uint32_t player_id, uint32_t window_id, const std::string& text)
{
	// Here we handle all text received by account manager
	const auto player = getPlayerByID(player_id);
	if (!player) 
	{
		return;
	}

	switch (window_id)
	{
		case AccountManager::ACCOUNT_NAME_TEXT_BOX: // Received Account Name
		{
			if (isAllowedRegistration(text) and not IOLoginData::accountExists(text))
			{
				player->setTempAccountName(text);
				player->sendModalWindow(CreateAccountManagerWindow(AccountManager::COMMON_PASSWORD));
				break;
			} // else

			player->sendModalWindow(CreateAccountManagerWindow(AccountManager::COMMON_ACCOUNT_FAILED));
			break;
		}

		case AccountManager::PASSWORD_TEXT_BOX: // Received Password (first time)
		{
			if (isAllowedRegistration(text)) 
			{
				player->setTempPassword(text);
				player->sendModalWindow(CreateAccountManagerWindow(AccountManager::COMMON_CONFIRMATION));
				break;
			} // else

			player->setTempPassword("");
			player->sendModalWindow(CreateAccountManagerWindow(AccountManager::COMMON_PASSWORD_FAILED));
			break;
		}

		case AccountManager::CONFIRMATION_TEXT_BOX: // Recieved Confirmation Password
		{
			if (text == player->getTempPassword())
			{
				Database& db = Database::getInstance();

				db.executeQuery(fmt::format("INSERT INTO `accounts` (`name`, `password`, `secret`, `type`, `premium_ends_at`, `email`, `creation`) "
					"VALUES ({:s}, {:s}, NULL, 1, 0, '', UNIX_TIMESTAMP())",
					db.escapeString(player->getTempAccountName()),
					db.escapeString(transformToSHA1(text))));

				player->sendModalWindow(CreateAccountManagerWindow(AccountManager::COMMON_SUCCESS));
				player->setTempAccountName("");
				player->setTempPassword("");
				break;
			} // else

			player->sendModalWindow(CreateAccountManagerWindow(AccountManager::COMMON_CONFIRMATION_FAILED));
			break;
		}
	}
}

void Game::onAccountManagerInput(const PlayerPtr& player, const uint32_t modalWindowId, const uint8_t button, const uint8_t choice)
{
	switch (button)
	{
		case AccountManager::DEFAULT_YES:
		{
			switch (modalWindowId)
			{
				case AccountManager::COMMON_LOGIN: // Welcome / Login Window
				{
					player->setAccountManagerLastState(modalWindowId);
					player->sendModalWindow(CreateAccountManagerWindow(AccountManager::COMMON_ACCOUNT));
					break;
				}

				case AccountManager::COMMON_ACCOUNT: // Account Window
				{
					player->sendAccountManagerTextWindow(AccountManager::ACCOUNT_NAME_TEXT_BOX, "account name");
					break;
				}

				case AccountManager::COMMON_PASSWORD: // Password Window
				{
					player->sendAccountManagerTextWindow(AccountManager::PASSWORD_TEXT_BOX, "password");
					break;
				}

				case AccountManager::COMMON_CONFIRMATION: // Password Confirmation Window
				{
					player->sendAccountManagerTextWindow(AccountManager::CONFIRMATION_TEXT_BOX, "retype password");
					break;
				}

				case AccountManager::COMMON_CANCEL:
				{
					player->kickPlayer(false);
					break;
				}

				case AccountManager::COMMON_SUCCESS:
				{
					player->kickPlayer(false);
					break;
				}

				case AccountManager::COMMON_ACCOUNT_FAILED:
				{
					player->sendAccountManagerTextWindow(AccountManager::ACCOUNT_NAME_TEXT_BOX, "account name");
					break;
				}

				case AccountManager::COMMON_PASSWORD_FAILED:
				{
					player->sendAccountManagerTextWindow(AccountManager::PASSWORD_TEXT_BOX, "password");
					break;
				}

				case AccountManager::COMMON_CONFIRMATION_FAILED:
				{
					player->sendAccountManagerTextWindow(AccountManager::CONFIRMATION_TEXT_BOX, "retype password");
					break;
				}

				default: // Shouldn't happen
				{
					player->kickPlayer(false);
					break;
				}
			}
			break;
		}

		case AccountManager::DEFAULT_NO:
		{	
			switch (modalWindowId)
			{
				case AccountManager::COMMON_LOGIN:
				case AccountManager::COMMON_ACCOUNT:
				case AccountManager::COMMON_PASSWORD:
				case AccountManager::COMMON_CONFIRMATION:
				case AccountManager::COMMON_ACCOUNT_FAILED:
				case AccountManager::COMMON_PASSWORD_FAILED:
				{
					player->setAccountManagerLastState(modalWindowId);
					player->sendModalWindow(CreateAccountManagerWindow(AccountManager::COMMON_CANCEL));
					break;
				}

				case AccountManager::COMMON_CONFIRMATION_FAILED:
				{
					player->sendAccountManagerTextWindow(AccountManager::PASSWORD_TEXT_BOX, "password");
					break;
				}

				case AccountManager::COMMON_CANCEL:
				{
					uint8_t last_state = player->getAccountManagerLastState();
					player->setAccountManagerLastState(0);
					player->sendModalWindow(CreateAccountManagerWindow(last_state));
					break;
				}
			}
			break;
		}

		case AccountManager::DEFAULT_RESET:
		{ 
			doAccountManagerReset(player->getID());
			break;
		}
	}
	player->onModalWindowHandled(modalWindowId);
}

void Game::doAccountManagerLogin(const PlayerPtr& player)
{

	// we do these resets incase player got here and it isn't when they logged in,
	// but rather from canceling and getting this window again or a doing a reset.
	player->setTempAccountName("");
	player->setTempPassword("");
	player->setAccountManagerLastState(0);

	auto isUnregisteredAccount = (player->getAccount() == 1);
	// make enums for the window ids

	if (isUnregisteredAccount)
	{
		auto welcomeWindow = CreateAccountManagerWindow(AccountManager::COMMON_LOGIN);
		player->sendModalWindow(welcomeWindow);
		return;
	}
	else // Account Manager on player's personal account
	{
		auto loginWindow = CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_LOGIN);
		player->sendModalWindow(loginWindow);
		return;
	}
}

bool Game::playerSaySpell(const PlayerPtr& player, const SpeakClasses type, const std::string& text)
{
	std::string words = text;

	TalkActionResult_t result = g_talkActions->playerSaySpell(player, type, words);
	if (result == TALKACTION_BREAK) {
		return true;
	}

	result = g_spells->playerSaySpell(player, words);
	if (result == TALKACTION_BREAK) {
		if (!g_config.getBoolean(ConfigManager::EMOTE_SPELLS)) {
			return internalCreatureSay(player, TALKTYPE_SAY, words, false);
		} else {
			return internalCreatureSay(player, TALKTYPE_MONSTER_SAY, words, false);
		}

	} else if (result == TALKACTION_FAILED) {
		return true;
	}

	return false;
}

void Game::playerWhisper(const PlayerPtr& player, const std::string& text)
{
	SpectatorVec spectators;
	map.getSpectators(spectators, player->getPosition(), false, false,
	              Map::maxClientViewportX, Map::maxClientViewportX,
	              Map::maxClientViewportY, Map::maxClientViewportY);

	//send to client
	for (const auto spectator : spectators) {
		if (const auto spectatorPlayer = spectator->getPlayer()) {
			if (!Position::areInRange<1, 1>(player->getPosition(), spectatorPlayer->getPosition())) {
				spectatorPlayer->sendCreatureSay(player, TALKTYPE_WHISPER, "pspsps");
			} else {
				spectatorPlayer->sendCreatureSay(player, TALKTYPE_WHISPER, text);
			}
		}
	}

	//event method
	for (const auto spectator : spectators) {
		spectator->onCreatureSay(player, TALKTYPE_WHISPER, text);
	}
}

bool Game::playerYell(const PlayerPtr& player, const std::string& text)
{
	if (player->hasCondition(CONDITION_YELLTICKS)) {
		player->sendCancelMessage(RETURNVALUE_YOUAREEXHAUSTED);
		return false;
	}

	if (uint32_t minimumLevel = g_config.getNumber(ConfigManager::YELL_MINIMUM_LEVEL); player->getLevel() < minimumLevel) {
		if (g_config.getBoolean(ConfigManager::YELL_ALLOW_PREMIUM)) {
			if (player->isPremium()) {
				internalCreatureSay(player, TALKTYPE_YELL, asUpperCaseString(text), false);
				return true;
			} else {
				player->sendTextMessage(MESSAGE_STATUS_SMALL, fmt::format("You may not yell unless you have reached level {:d} or have a premium account.", minimumLevel));
			}
		} else {
			player->sendTextMessage(MESSAGE_STATUS_SMALL, fmt::format("You may not yell unless you have reached level {:d}.", minimumLevel));
		}
		return false;
	}

	if (player->getAccountType() < ACCOUNT_TYPE_GAMEMASTER) {
		Condition* condition = Condition::createCondition(CONDITIONID_DEFAULT, CONDITION_YELLTICKS, 30000, 0);
		player->addCondition(condition);
	}

	internalCreatureSay(player, TALKTYPE_YELL, asUpperCaseString(text), false);
	return true;
}

bool Game::playerSpeakTo(const PlayerPtr& player, SpeakClasses type, const std::string& receiver,
                         const std::string& text)
{
	const auto toPlayer = getPlayerByName(receiver);
	if (!toPlayer) {
		player->sendTextMessage(MESSAGE_STATUS_SMALL, "A player with this name is not online.");
		return false;
	}

	if (type == TALKTYPE_PRIVATE_RED_TO && (player->hasFlag(PlayerFlag_CanTalkRedPrivate) || player->getAccountType() >= ACCOUNT_TYPE_GAMEMASTER)) {
		type = TALKTYPE_PRIVATE_RED_FROM;
	} else {
		type = TALKTYPE_PRIVATE_FROM;
	}

	if (!player->isAccessPlayer()) {
		if (uint32_t minimumLevel = g_config.getNumber(ConfigManager::MINIMUM_LEVEL_TO_SEND_PRIVATE); player->getLevel() < minimumLevel) {
			if (g_config.getBoolean(ConfigManager::PREMIUM_TO_SEND_PRIVATE)) {
				if (!player->isPremium()) {
					player->sendTextMessage(
						MESSAGE_STATUS_SMALL,
						fmt::format(
							"You may not send private messages unless you have reached level {:d} or have a premium account.",
							minimumLevel));
					return false;
				}
			}
			else {
				player->sendTextMessage(
					MESSAGE_STATUS_SMALL,
					fmt::format("You may not send private messages unless you have reached level {:d}.", minimumLevel));
				return false;
			}
		}
	}

	toPlayer->sendPrivateMessage(player, type, text);
	toPlayer->onCreatureSay(player, type, text);

	if (toPlayer->isInGhostMode() && !player->canSeeGhostMode(toPlayer)) {
		player->sendTextMessage(MESSAGE_STATUS_SMALL, "A player with this name is not online.");
	} else {
		player->sendTextMessage(MESSAGE_STATUS_SMALL, fmt::format("Message sent to {:s}.", toPlayer->getName()));
	}
	return true;
}

void Game::playerSpeakToNpc(const PlayerPtr& player, const std::string& text)
{
	SpectatorVec spectators;
	map.getSpectators(spectators, player->getPosition());
	for (const auto spectator : spectators) {
		if (spectator->getNpc()) {
			spectator->onCreatureSay(player, TALKTYPE_PRIVATE_PN, text);
		}
	}
}

//--
bool Game::canThrowObjectTo(const Position& fromPos, const Position& toPos, const bool checkLineOfSight /*= true*/, const bool sameFloor /*= false*/,
                            const int32_t rangex /*= Map::maxClientViewportX*/, const int32_t rangey /*= Map::maxClientViewportY*/)
{
	return map.canThrowObjectTo(fromPos, toPos, checkLineOfSight, sameFloor, rangex, rangey);
}

bool Game::isSightClear(const Position& fromPos, const Position& toPos, const bool sameFloor /*= false*/)
{
	return map.isSightClear(fromPos, toPos, sameFloor);
}

bool Game::internalCreatureTurn(const CreaturePtr& creature, const Direction dir)
{
	if (creature->getDirection() == dir) {
		return false;
	}

	creature->setDirection(dir);

	//send to client
	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendCreatureTurn(creature);
	}
	return true;
}

bool Game::internalCreatureSay(const CreaturePtr& creature, const SpeakClasses type, const std::string& text,
                               const bool ghostMode, SpectatorVec* spectatorsPtr/* = nullptr*/, const Position* pos/* = nullptr*/, const bool echo/* = false*/)
{
	if (text.empty()) {
		return false;
	}

	if (!pos) {
		pos = &creature->getPosition();
	}

	SpectatorVec spectators;

	if (!spectatorsPtr || spectatorsPtr->empty()) {
		// This somewhat complex construct ensures that the cached SpectatorVec
		// is used if available and if it can be used, else a local vector is
		// used (hopefully the compiler will optimize away the construction of
		// the temporary when it's not used).
		if (type != TALKTYPE_YELL && type != TALKTYPE_MONSTER_YELL) {
			map.getSpectators(spectators, *pos, false, false,
			              Map::maxClientViewportX, Map::maxClientViewportX,
			              Map::maxClientViewportY, Map::maxClientViewportY);
		} else {
			map.getSpectators(spectators, *pos, true, false, 18, 18, 14, 14);
		}
	} else {
		spectators = (*spectatorsPtr);
	}

	//send to client
	for (const auto spectator : spectators) {
		if (const auto tmpPlayer = spectator->getPlayer()) {
			if (!ghostMode || tmpPlayer->canSeeCreature(creature)) {
				tmpPlayer->sendCreatureSay(creature, type, text, pos);
			}
		}
	}

	//event method
	if (!echo) {
		for (const auto spectator : spectators) {
			spectator->onCreatureSay(creature, type, text);
			if (creature != spectator) {
				g_events->eventCreatureOnHear(spectator, creature, text, type);
			}
		}
	}
	return true;
}

void Game::checkCreatureWalk(const uint32_t creatureId)
{
	const auto creature = getCreatureByID(creatureId);
	if (creature && creature->getHealth() > 0) {
		creature->onWalk();
		cleanup();
	}
}

void Game::updateCreatureWalk(const uint32_t creatureId)
{
	const auto creature = getCreatureByID(creatureId);
	if (creature && creature->getHealth() > 0) {
		creature->goToFollowCreature();
	}
}

void Game::checkCreatureAttack(const uint32_t creatureId)
{
	const auto creature = getCreatureByID(creatureId);
	if (creature && creature->getHealth() > 0) {
		creature->onAttacking(0);
	}
}

void Game::addCreatureCheck(const CreaturePtr& creature)
{
	creature->creatureCheck = true;

	if (creature->inCheckCreaturesVector) {
		// already in a vector
		return;
	}

	creature->inCheckCreaturesVector = true;
	checkCreatureLists[uniform_random(0, EVENT_CREATURECOUNT - 1)].push_back(creature);
}

void Game::removeCreatureCheck(const CreaturePtr& creature)
{
	if (creature->inCheckCreaturesVector) {
		creature->creatureCheck = false;
	}
}

void Game::checkCreatures(const size_t index)
{
	g_scheduler.addEvent(createSchedulerTask(EVENT_CHECK_CREATURE_INTERVAL, [=, this]() { checkCreatures((index + 1) % EVENT_CREATURECOUNT); }));

	auto& checkCreatureList = checkCreatureLists[index];
	auto it = checkCreatureList.begin();
	const auto end = checkCreatureList.end();
	while (it != end) {
		if (const auto creature = *it; creature->creatureCheck) {
			if (creature->getHealth() > 0) {
				creature->onThink(EVENT_CREATURE_THINK_INTERVAL);
				creature->onAttacking(EVENT_CREATURE_THINK_INTERVAL);
				creature->executeConditions(EVENT_CREATURE_THINK_INTERVAL);
			}
			++it;
		} else {
			creature->inCheckCreaturesVector = false;
			it = checkCreatureList.erase(it);
			// ReleaseCreature(creature);
		}
	}

	cleanup();
}

void Game::changeSpeed(const CreaturePtr& creature, const int32_t varSpeedDelta)
{
	int32_t varSpeed = creature->getSpeed() - creature->getBaseSpeed();
	varSpeed += varSpeedDelta;

	creature->setSpeed(varSpeed);

	//send to clients
	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), false, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendChangeSpeed(creature, creature->getStepSpeed());
	}
}

void Game::internalCreatureChangeOutfit(const CreaturePtr& creature, const Outfit_t& outfit)
{
	if (!g_events->eventCreatureOnChangeOutfit(creature, outfit)) {
		return;
	}

	creature->setCurrentOutfit(outfit);

	if (creature->isInvisible()) {
		return;
	}

	//send to clients
	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendCreatureChangeOutfit(creature, outfit);
	}
}

void Game::internalCreatureChangeVisible(const CreaturePtr& creature, bool visible)
{
	//send to clients
	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendCreatureChangeVisible(creature, visible);
	}
}

void Game::changeLight(const CreatureConstPtr& creature)
{
	//send to clients
	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendCreatureLight(creature);
	}
}

bool Game::combatBlockHit(CombatDamage& damage, const CreaturePtr& attacker, const CreaturePtr& target, const bool checkDefense, const bool checkArmor, const bool field, const bool ignoreResistances /*= false */)
{
	if (damage.primary.type == COMBAT_NONE && damage.secondary.type == COMBAT_NONE) {
		return true;
	}

	if (target->getPlayer() && target->isInGhostMode()) {
		return true;
	}

	if (damage.primary.value > 0) {
		return false;
	}

	static const auto sendBlockEffect = [this](const BlockType_t blockType, const CombatType_t combatType, const Position& targetPos) {
		if (blockType == BLOCK_DEFENSE) {
			addMagicEffect(targetPos, CONST_ME_POFF);
		} else if (blockType == BLOCK_ARMOR) {
			addMagicEffect(targetPos, CONST_ME_BLOCKHIT);
		} else if (blockType == BLOCK_IMMUNITY) {
			uint8_t hitEffect = 0;
			switch (combatType) {
				case COMBAT_UNDEFINEDDAMAGE: {
					return;
				}
				case COMBAT_ENERGYDAMAGE:
				case COMBAT_FIREDAMAGE:
				case COMBAT_PHYSICALDAMAGE:
				case COMBAT_ICEDAMAGE:
				case COMBAT_DEATHDAMAGE: {
					hitEffect = CONST_ME_BLOCKHIT;
					break;
				}
				case COMBAT_EARTHDAMAGE: {
					hitEffect = CONST_ME_GREEN_RINGS;
					break;
				}
				case COMBAT_HOLYDAMAGE: {
					hitEffect = CONST_ME_HOLYDAMAGE;
					break;
				}
				default: {
					hitEffect = CONST_ME_POFF;
					break;
				}
			}
			addMagicEffect(targetPos, hitEffect);
		}
	};

	BlockType_t primaryBlockType, secondaryBlockType;
	if (damage.primary.type != COMBAT_NONE) {
		damage.primary.value = -damage.primary.value;
		primaryBlockType = target->blockHit(attacker, damage.primary.type, damage.primary.value, checkDefense, checkArmor, field, ignoreResistances);
		if (attacker && target && (damage.primary.type != COMBAT_HEALING)) {
			g_events->eventCreatureOnAttack(attacker, target, primaryBlockType, damage.primary.type, damage.origin, damage.critical, damage.leeched);
			g_events->eventCreatureOnDefend(target, attacker, primaryBlockType, damage.primary.type, damage.origin, damage.critical, damage.leeched);

			if (const auto& aggressor = attacker->getPlayer()) {
				for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot) {
					const auto& item = aggressor->getInventoryItem(static_cast<slots_t>(slot));
					if ((!item) or (item->getAttack() <= 0) ) {
						continue;
					}
					g_events->eventItemOnAttack(item, aggressor, target, primaryBlockType, damage.primary.type, damage.origin, damage.critical, damage.leeched);
				}
			}

			if (const auto& victim = target->getPlayer()) {
				for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot) {
					const auto& item = victim->getInventoryItem(static_cast<slots_t>(slot));
					if ((!item) or (item->getDefense() <= 0 and item->getArmor() <= 0)  ){
						continue;
					}
					g_events->eventItemOnDefend(item, victim, attacker, primaryBlockType, damage.primary.type, damage.origin, damage.critical, damage.leeched);
				}
			}
		}
		damage.primary.value = -damage.primary.value;
		sendBlockEffect(primaryBlockType, damage.primary.type, target->getPosition());
	} else {
		primaryBlockType = BLOCK_NONE;
	}

	if (damage.secondary.type != COMBAT_NONE) {
		damage.secondary.value = -damage.secondary.value;
		secondaryBlockType = target->blockHit(attacker, damage.secondary.type, damage.secondary.value, false, false, field, ignoreResistances);
		if (attacker && target && (damage.primary.type != COMBAT_HEALING)) {
			g_events->eventCreatureOnAttack(attacker, target, secondaryBlockType, damage.secondary.type, damage.origin, damage.critical, damage.leeched);
			g_events->eventCreatureOnDefend(target, attacker, secondaryBlockType, damage.secondary.type, damage.origin, damage.critical, damage.leeched);

			if (const auto aggressor = attacker->getPlayer()) {
				for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot) {
					const auto item = aggressor->getInventoryItem(static_cast<slots_t>(slot));
					if (!item) {
						continue;
					}
					if (item->getAttack() > 0) {
						g_events->eventItemOnAttack(item, aggressor, target, secondaryBlockType, damage.secondary.type, damage.origin, damage.critical, damage.leeched);
					}
				}
			}

			if (const auto victim = target->getPlayer()) {
				for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot) {
					const auto item = victim->getInventoryItem(static_cast<slots_t>(slot));
					if (!item) {
						continue;
					}

					if (item->getDefense() > 0 || item->getArmor() > 0) {
						g_events->eventItemOnDefend(item, victim, attacker, secondaryBlockType, damage.secondary.type, damage.origin, damage.critical, damage.leeched);
					}
				}
			}
		}
		damage.secondary.value = -damage.secondary.value;
		sendBlockEffect(secondaryBlockType, damage.secondary.type, target->getPosition());
	} else {
		secondaryBlockType = BLOCK_NONE;
	}

	damage.blockType = primaryBlockType;

	return (primaryBlockType != BLOCK_NONE) && (secondaryBlockType != BLOCK_NONE);
}

void Game::combatGetTypeInfo(const CombatType_t combatType, const CreaturePtr& target, TextColor_t& color, uint8_t& effect)
{
	switch (combatType) {
		case COMBAT_PHYSICALDAMAGE: {
			ItemPtr splash = nullptr;
			switch (target->getRace()) {
				case RACE_VENOM:
					color = TEXTCOLOR_LIGHTGREEN;
					effect = CONST_ME_HITBYPOISON;
					splash = Item::CreateItem(ITEM_SMALLSPLASH, FLUID_SLIME);
					break;
				case RACE_BLOOD:
					color = TEXTCOLOR_RED;
					effect = CONST_ME_DRAWBLOOD;
					if (const auto tile = target->getTile()) {
						if (!tile->hasFlag(TILESTATE_PROTECTIONZONE)) {
							splash = Item::CreateItem(ITEM_SMALLSPLASH, FLUID_BLOOD);
						}
					}
					break;
				case RACE_UNDEAD:
					color = TEXTCOLOR_LIGHTGREY;
					effect = CONST_ME_HITAREA;
					break;
				case RACE_FIRE:
					color = TEXTCOLOR_ORANGE;
					effect = CONST_ME_DRAWBLOOD;
					break;
				case RACE_ENERGY:
					color = TEXTCOLOR_ELECTRICPURPLE;
					effect = CONST_ME_ENERGYHIT;
					break;
				default:
					color = TEXTCOLOR_NONE;
					effect = CONST_ME_NONE;
					break;
			}

			if (splash) {
				CylinderPtr t_tile = target->getTile();
				internalAddItem(t_tile, splash, INDEX_WHEREEVER, FLAG_NOLIMIT);
				startDecay(splash);
			}

			break;
		}

		case COMBAT_ENERGYDAMAGE: {
			color = TEXTCOLOR_ELECTRICPURPLE;
			effect = CONST_ME_ENERGYHIT;
			break;
		}

		case COMBAT_EARTHDAMAGE: {
			color = TEXTCOLOR_LIGHTGREEN;
			effect = CONST_ME_GREEN_RINGS;
			break;
		}

		case COMBAT_DROWNDAMAGE: {
			color = TEXTCOLOR_LIGHTBLUE;
			effect = CONST_ME_LOSEENERGY;
			break;
		}
		
		case COMBAT_FIREDAMAGE: {
			color = TEXTCOLOR_ORANGE;
			effect = CONST_ME_HITBYFIRE;
			break;
		}
		
		case COMBAT_ICEDAMAGE: {
			color = TEXTCOLOR_SKYBLUE;
			effect = CONST_ME_ICEATTACK;
			break;
		}
		
		case COMBAT_HOLYDAMAGE: {
			color = TEXTCOLOR_YELLOW;
			effect = CONST_ME_HOLYDAMAGE;
			break;
		}
		
		case COMBAT_DEATHDAMAGE: {
			color = TEXTCOLOR_DARKRED;
			effect = CONST_ME_SMALLCLOUDS;
			break;
		}
		
		case COMBAT_LIFEDRAIN: {
			color = TEXTCOLOR_RED;
			effect = CONST_ME_MAGIC_RED;
			break;
		}
		
		default: {
			color = TEXTCOLOR_NONE;
			effect = CONST_ME_NONE;
			break;
		}
	}
}

bool Game::combatChangeHealth(const CreaturePtr& attacker, const CreaturePtr& target, CombatDamage& damage)
{
	if (damage.primary.value == 0 && damage.secondary.value == 0) {
		return true;
	}

	const auto& attackerPlayer = attacker && attacker->getPlayer() ? attacker->getPlayer() : nullptr;
	auto targetPlayer = target && target->getPlayer() ? target->getPlayer() : nullptr;

	if (attackerPlayer && targetPlayer && attackerPlayer->getSkull() == SKULL_BLACK && attackerPlayer->getSkullClient(targetPlayer) == SKULL_NONE) {
		return false;
	}

	// Early exit if the target is already dead
	if (target->getHealth() <= 0) {
		return false;
	}

	// Handle Lua events immediately
	if (const auto& events = target->getCreatureEvents(CREATURE_EVENT_HEALTHCHANGE); !events.empty()) {
		for (const auto& creatureEvent : events) {
			creatureEvent->executeHealthChange(target, attacker, damage);
		}
	}

	const Position& targetPos = target->getPosition();

	// Healing
	if (damage.primary.value > 0) {
		int32_t realHealthChange = target->getHealth();
		target->gainHealth(attacker, damage.primary.value);
		realHealthChange = target->getHealth() - realHealthChange;

		// Reward boss healing contribution
		if (targetPlayer) {
			for (const auto& monsterId : g_game.rewardBossTracking | std::views::keys) {
				if (const auto& monster = getMonsterByID(monsterId); monster && monster->isRewardBoss()) {
					const Position& monsterPos = monster->getPosition();
					if (monsterPos.z == targetPos.z) {
						int32_t dx = targetPos.x - monsterPos.x;
						int32_t dy = targetPos.y - monsterPos.y;
						if (dx * dx + dy * dy < 49) { // 7^2 = 49
							uint32_t playerGuid = targetPlayer->getGUID();
							rewardBossTracking[monsterId].playerScoreTable[playerGuid].damageTaken += realHealthChange * g_config.getFloat(ConfigManager::REWARD_RATE_HEALING_DONE);
						}
					}
				}
			}
		}

		if (realHealthChange > 0 && !target->isInGhostMode()) {
			const auto& damageString = std::to_string(realHealthChange) + " hitpoint" + ((realHealthChange != 1) ? "s" : "");
			const auto& targetNameDesc = target->getNameDescription();
			const auto& attackerNameDesc = attacker ? attacker->getNameDescription() : "";
			std::string spectatorMessage;

			if (!attacker) {
				spectatorMessage = targetNameDesc + " is healed for " + damageString;
			}
			else if (attacker == target) {
				spectatorMessage = attackerNameDesc + " heals " + (targetPlayer ? (targetPlayer->getSex() == PLAYERSEX_FEMALE ? "her" : "him") : "it") + "self for " + damageString + ".";
			}
			else {
				spectatorMessage = attackerNameDesc + " heals " + targetNameDesc + " for " + damageString + ".";
			}
			spectatorMessage[0] = std::toupper(spectatorMessage[0]);

			TextMessage message;
			message.position = targetPos;
			message.primary.value = realHealthChange;
			message.primary.color = TEXTCOLOR_PASTELRED;

			SpectatorVec spectators;
			map.getSpectators(spectators, targetPos, false, true);

			for (const auto& spectator : spectators) {
				auto spectatorPlayer = std::static_pointer_cast<Player>(spectator);
				if (spectatorPlayer == attackerPlayer && attackerPlayer != targetPlayer) {
					message.type = MESSAGE_HEALED;
					message.text = "You heal " + targetNameDesc + " for " + damageString + ".";
				}
				else if (spectatorPlayer == targetPlayer) {
					message.type = MESSAGE_HEALED;
					message.text = !attacker ? "You are healed for " + damageString + "." :
						(targetPlayer == attackerPlayer ? "You heal yourself for " + damageString + "." :
							"You are healed by " + attackerNameDesc + " for " + damageString + ".");
				}
				else {
					message.type = MESSAGE_HEALED_OTHERS;
					message.text = spectatorMessage;
				}
				spectatorPlayer->sendTextMessage(message);
			}
		}
	}
	else {
		if (!target->isAttackable()) {
			if (!target->isInGhostMode()) {
				addMagicEffect(targetPos, CONST_ME_POFF);
			}
			return true;
		}

		damage.primary.value = std::abs(damage.primary.value);
		damage.secondary.value = std::abs(damage.secondary.value);

		int32_t healthChange = damage.primary.value + damage.secondary.value;
		if (healthChange == 0) {
			return true;
		}

		TextMessage message;
		message.position = targetPos;

		SpectatorVec spectators;
		map.getSpectators(spectators, targetPos, true, true);

		if (targetPlayer && targetPlayer->hasCondition(CONDITION_MANASHIELD) && damage.primary.type != COMBAT_UNDEFINEDDAMAGE) {
			if (int32_t manaDamage = std::min<int32_t>(targetPlayer->getMana(), healthChange); manaDamage != 0) {
				if (const auto& events = target->getCreatureEvents(CREATURE_EVENT_MANACHANGE); !events.empty()) {
					for (const auto& creatureEvent : events) {
						creatureEvent->executeManaChange(target, attacker, damage);
					}
					healthChange = damage.primary.value + damage.secondary.value;
					if (healthChange == 0) {
						return true;
					}
					manaDamage = std::min<int32_t>(targetPlayer->getMana(), healthChange);
				}

				// Drain mana and add visual effect
				targetPlayer->drainMana(attacker, manaDamage);
				addMagicEffect(spectators, targetPos, CONST_ME_LOSEENERGY);

				const auto& targetNameDesc = target->getNameDescription();
				const auto& attackerNameDesc = attacker ? attacker->getNameDescription() : "";
				std::string spectatorMessage;

				if (!attacker) {
					spectatorMessage = targetNameDesc + " loses " + std::to_string(manaDamage) + " mana.";
				}
				else if (attacker == target) {
					spectatorMessage = targetNameDesc + " loses " + std::to_string(manaDamage) + " mana due to " +
						(targetPlayer->getSex() == PLAYERSEX_FEMALE ? "her" : "his") + " own attack.";
				}
				else {
					spectatorMessage = targetNameDesc + " loses " + std::to_string(manaDamage) + " mana due to an attack by " +	attackerNameDesc + ".";
				}
				spectatorMessage[0] = std::toupper(spectatorMessage[0]);

				message.primary.value = manaDamage;
				message.primary.color = TEXTCOLOR_BLUE;

				// Notify spectators about mana loss
				for (const auto& spectator : spectators) {
					PlayerPtr spectatorPlayer = std::static_pointer_cast<Player>(spectator);
					if (spectatorPlayer->getPosition().z != targetPos.z) {
						continue;
					}

					if (spectatorPlayer == attackerPlayer && attackerPlayer != targetPlayer) {
						message.type = MESSAGE_DAMAGE_DEALT;
						message.text = targetNameDesc + " loses " + std::to_string(manaDamage) + " mana due to your attack.";
						message.text[0] = std::toupper(message.text[0]);
					}
					else if (spectatorPlayer == targetPlayer) {
						message.type = MESSAGE_DAMAGE_RECEIVED;
						message.text = !attacker ? "You lose " + std::to_string(manaDamage) + " mana." :
							(targetPlayer == attackerPlayer ? "You lose " + std::to_string(manaDamage) + " mana due to your own attack." :
								"You lose " + std::to_string(manaDamage) + " mana due to an attack by " + attackerNameDesc + ".");
					}
					else {
						message.type = MESSAGE_DAMAGE_OTHERS;
						message.text = spectatorMessage;
					}
					spectatorPlayer->sendTextMessage(message);
				}

				damage.primary.value -= manaDamage;
				if (damage.primary.value < 0) {
					damage.secondary.value = std::max<int32_t>(0, damage.secondary.value + damage.primary.value);
					damage.primary.value = 0;
				}
			}
		}

		// Calculate total remaining damage
		int32_t realDamage = damage.primary.value + damage.secondary.value;
		if (realDamage == 0) {
			return true;
		}

		// Cap damage at target's health
		int32_t targetHealth = target->getHealth();
		if (damage.primary.value >= targetHealth) {
			damage.primary.value = targetHealth;
			damage.secondary.value = 0;
		}
		else if (damage.secondary.value) {
			damage.secondary.value = std::min<int32_t>(damage.secondary.value, targetHealth - damage.primary.value);
		}

		realDamage = damage.primary.value + damage.secondary.value;
		if (realDamage == 0) {
			return true;
		}

		message.primary.value = damage.primary.value;
		message.secondary.value = damage.secondary.value;

		uint8_t hitEffect;
		if (message.primary.value) {
			combatGetTypeInfo(damage.primary.type, target, message.primary.color, hitEffect);
			if (hitEffect != CONST_ME_NONE) {
				addMagicEffect(spectators, targetPos, hitEffect);
			}
		}

		if (message.secondary.value) {
			combatGetTypeInfo(damage.secondary.type, target, message.secondary.color, hitEffect);
			if (hitEffect != CONST_ME_NONE) {
				addMagicEffect(spectators, targetPos, hitEffect);
			}
		}

		if (message.primary.color != TEXTCOLOR_NONE || message.secondary.color != TEXTCOLOR_NONE) {
			const auto& damageString = std::to_string(realDamage) + " hitpoint" + (realDamage && realDamage != 1 ? "s" : "");
			const auto& targetNameDesc = target->getNameDescription();
			const auto& attackerNameDesc = attacker ? attacker->getNameDescription() : "";
			std::string spectatorMessage;

			if (!attacker) {
				spectatorMessage = targetNameDesc + " loses " + damageString + ".";
			}
			else if (attacker == target) {
				spectatorMessage = targetNameDesc + " loses " + damageString + " due to " +
					(targetPlayer ? (targetPlayer->getSex() == PLAYERSEX_FEMALE ? "her" : "his") : "its") +	" own attack.";
			}
			else {
				spectatorMessage = targetNameDesc + " loses " + damageString + " due to an attack by " + attackerNameDesc + ".";
			}
			spectatorMessage[0] = std::toupper(spectatorMessage[0]);

			for (const auto& spectator : spectators) {
				const auto& tmpPlayer = spectator->getPlayer();
				if (tmpPlayer->getPosition().z != targetPos.z) {
					continue;
				}

				if (tmpPlayer == attackerPlayer && attackerPlayer != targetPlayer) {
					message.type = MESSAGE_DAMAGE_DEALT;
					message.text = targetNameDesc + " loses " + damageString + " due to your attack.";
					message.text[0] = std::toupper(message.text[0]);
				}
				else if (tmpPlayer == targetPlayer) {
					message.type = MESSAGE_DAMAGE_RECEIVED;
					message.text = !attacker ? "You lose " + damageString + "." :
						(targetPlayer == attackerPlayer ? "You lose " + damageString + " due to your own attack." :
							"You lose " + damageString + " due to an attack by " + attackerNameDesc + ".");
				}
				else {
					message.type = MESSAGE_DAMAGE_OTHERS;
					message.text = spectatorMessage;
				}
				tmpPlayer->sendTextMessage(message);
			}
		}

		// Reward boss tracking: player attacking boss
		if (target && target->getMonster() && target->getMonster()->isRewardBoss()) {
			uint32_t monsterId = target->getMonster()->getID();
			if (!rewardBossTracking.contains(monsterId)) {
				rewardBossTracking[monsterId] = RewardBossContributionInfo();
			}
			if (attackerPlayer) {
				uint32_t playerGuid = attackerPlayer->getGUID();
				rewardBossTracking[monsterId].playerScoreTable[playerGuid].damageDone += realDamage * g_config.getFloat(ConfigManager::REWARD_RATE_DAMAGE_DONE);
			}
		}

		// Reward boss tracking: boss attacking player
		if (attacker && attacker->getMonster() && attacker->getMonster()->isRewardBoss()) {
			uint32_t monsterId = attacker->getMonster()->getID();
			if (!rewardBossTracking.contains(monsterId)) {
				rewardBossTracking[monsterId] = RewardBossContributionInfo();
			}
			if (target && target->getPlayer()) {
				uint32_t playerGuid = target->getPlayer()->getGUID();
				rewardBossTracking[monsterId].playerScoreTable[playerGuid].damageTaken += realDamage * g_config.getFloat(ConfigManager::REWARD_RATE_DAMAGE_TAKEN);
			}
		}

		// Check for death and execute prepare-death events
		if (realDamage >= targetHealth) {
			for (const auto& creatureEvent : target->getCreatureEvents(CREATURE_EVENT_PREPAREDEATH)) {
				if (!creatureEvent->executeOnPrepareDeath(target, attacker)) {
					return false;
				}
			}
		}

		// Apply damage and update spectators
		target->drainHealth(attacker, realDamage);
		addCreatureHealth(spectators, target);
	}

	return true;
}


bool Game::combatChangeMana(const CreaturePtr& attacker, const CreaturePtr& target, CombatDamage& damage)
{
	const auto targetPlayer = target->getPlayer();
	if (!targetPlayer) {
		return true;
	}

	int32_t manaChange = damage.primary.value + damage.secondary.value;
	if (manaChange > 0) {
		if (attacker) {
			const auto& attackerPlayer = attacker->getPlayer();
			if (attackerPlayer && attackerPlayer->getSkull() == SKULL_BLACK && attackerPlayer->getSkullClient(target) == SKULL_NONE) {
				return false;
			}
		}

		const auto& events = target->getCreatureEvents(CREATURE_EVENT_MANACHANGE);
		if (!events.empty()) {
			for (const auto& creatureEvent : events) {
				creatureEvent->executeManaChange(target, attacker, damage);
			}
		}

		int32_t realManaChange = targetPlayer->getMana();
		targetPlayer->changeMana(manaChange);
		realManaChange = targetPlayer->getMana() - realManaChange;

		if (realManaChange > 0 && !targetPlayer->isInGhostMode()) {
			TextMessage message(MESSAGE_HEALED, "You gain " + std::to_string(realManaChange) + " mana.");
			message.position = target->getPosition();
			message.primary.value = realManaChange;
			message.primary.color = TEXTCOLOR_MAYABLUE;
			targetPlayer->sendTextMessage(message);
		}
	}
	else {
		const Position& targetPos = target->getPosition();
		if (!target->isAttackable()) {
			if (!target->isInGhostMode()) {
				addMagicEffect(targetPos, CONST_ME_POFF);
			}
			return false;
		}

		PlayerPtr attackerPlayer;
		if (attacker) {
			attackerPlayer = attacker->getPlayer();
		}
		else {
			attackerPlayer = nullptr;
		}

		if (attackerPlayer && attackerPlayer->getSkull() == SKULL_BLACK && attackerPlayer->getSkullClient(targetPlayer) == SKULL_NONE) {
			return false;
		}

		int32_t manaLoss = std::min<int32_t>(targetPlayer->getMana(), -manaChange);
		BlockType_t blockType = target->blockHit(attacker, COMBAT_MANADRAIN, manaLoss);
		if (blockType != BLOCK_NONE) {
			addMagicEffect(targetPos, CONST_ME_POFF);
			return false;
		}

		if (manaLoss <= 0) {
			return true;
		}

		const auto& events = target->getCreatureEvents(CREATURE_EVENT_MANACHANGE);
		if (!events.empty()) {
			for (const auto& creatureEvent : events) {
				creatureEvent->executeManaChange(target, attacker, damage);
			}
		}

		targetPlayer->drainMana(attacker, manaLoss);

		const auto& targetNameDesc = target->getNameDescription();
		const auto& attackerNameDesc = attacker ? attacker->getNameDescription() : "";
		std::string spectatorMessage;

		if (!attacker) {
			spectatorMessage = targetNameDesc + " loses " + std::to_string(manaLoss) + " mana.";
		}
		else if (attacker == target) {
			spectatorMessage = targetNameDesc + " loses " + std::to_string(manaLoss) + " mana due to " +
				(targetPlayer->getSex() == PLAYERSEX_FEMALE ? "her" : "his") + " own attack.";
		}
		else {
			spectatorMessage = targetNameDesc + " loses " + std::to_string(manaLoss) + " mana due to an attack by " +
				attackerNameDesc + ".";
		}
		spectatorMessage[0] = std::toupper(spectatorMessage[0]);

		TextMessage message;
		message.position = targetPos;
		message.primary.value = manaLoss;
		message.primary.color = TEXTCOLOR_BLUE;

		SpectatorVec spectators;
		map.getSpectators(spectators, targetPos, false, true);
		for (const auto& spectator : spectators) {
			PlayerPtr spectatorPlayer = std::static_pointer_cast<Player>(spectator);
			if (spectatorPlayer == attackerPlayer && attackerPlayer != targetPlayer) {
				message.type = MESSAGE_DAMAGE_DEALT;
				message.text = targetNameDesc + " loses " + std::to_string(manaLoss) + " mana due to your attack.";
				message.text[0] = std::toupper(message.text[0]);
			}
			else if (spectatorPlayer == targetPlayer) {
				message.type = MESSAGE_DAMAGE_RECEIVED;
				if (!attacker) {
					message.text = "You lose " + std::to_string(manaLoss) + " mana.";
				}
				else if (targetPlayer == attackerPlayer) {
					message.text = "You lose " + std::to_string(manaLoss) + " mana due to your own attack.";
				}
				else {
					message.text = "You lose " + std::to_string(manaLoss) + " mana due to an attack by " + attackerNameDesc + ".";
				}
			}
			else {
				message.type = MESSAGE_DAMAGE_OTHERS;
				message.text = spectatorMessage;
			}
			spectatorPlayer->sendTextMessage(message);
		}
	}

	return true;
}

void Game::addCreatureHealth(const CreatureConstPtr& target)
{
	SpectatorVec spectators;
	map.getSpectators(spectators, target->getPosition(), true, true);
	addCreatureHealth(spectators, target);
}

void Game::addCreatureHealth(const SpectatorVec& spectators, const CreatureConstPtr& target)
{
	for (const auto spectator : spectators) {
		if (const auto tmpPlayer = spectator->getPlayer()) {
			tmpPlayer->sendCreatureHealth(target);
		}
	}
}

void Game::addMagicEffect(const Position& pos, const uint8_t effect)
{
	SpectatorVec spectators;
	map.getSpectators(spectators, pos, true, true);
	addMagicEffect(spectators, pos, effect);
}

void Game::addMagicEffect(const SpectatorVec& spectators, const Position& pos, const uint8_t effect)
{
	for (const auto spectator : spectators) {
		if (const auto tmpPlayer = spectator->getPlayer()) {
			tmpPlayer->sendMagicEffect(pos, effect);
		}
	}
}

void Game::addDistanceEffect(const Position& fromPos, const Position& toPos, const uint8_t effect)
{
	SpectatorVec spectators, toPosSpectators;
	map.getSpectators(spectators, fromPos, true, true);
	map.getSpectators(toPosSpectators, toPos, true, true);
	spectators.addSpectators(toPosSpectators);

	addDistanceEffect(spectators, fromPos, toPos, effect);
}

void Game::addDistanceEffect(const SpectatorVec& spectators, const Position& fromPos, const Position& toPos, uint8_t effect)
{
	for (const auto spectator : spectators) {
		if (const auto tmpPlayer = spectator->getPlayer()) {
			tmpPlayer->sendDistanceShoot(fromPos, toPos, effect);
		}
	}
}

void Game::setAccountStorageValue(const uint32_t accountId, const uint32_t key, const int32_t value)
{
	if (value == -1) {
		accountStorageMap[accountId].erase(key);
		return;
	}

	accountStorageMap[accountId][key] = value;
}

int32_t Game::getAccountStorageValue(const uint32_t accountId, const uint32_t key) const
{
	const auto& accountMapIt = accountStorageMap.find(accountId);
	if (accountMapIt != accountStorageMap.end()) {
		const auto& storageMapIt = accountMapIt->second.find(key);
		if (storageMapIt != accountMapIt->second.end()) {
			return storageMapIt->second;
		}
	}
	return -1;
}

void Game::loadAccountStorageValues()
{
	Database& db = Database::getInstance();

	DBResult_ptr result;
	if ((result = db.storeQuery("SELECT `account_id`, `key`, `value` FROM `account_storage`"))) {
		do {
			g_game.setAccountStorageValue(result->getNumber<uint32_t>("account_id"), result->getNumber<uint32_t>("key"), result->getNumber<int32_t>("value"));
		} while (result->next());
	}
}

bool Game::saveAccountStorageValues() const
{
	DBTransaction transaction;
	Database& db = Database::getInstance();

	if (!transaction.begin()) {
		return false;
	}

	if (!db.executeQuery("DELETE FROM `account_storage`")) {
		return false;
	}

	for (const auto& accountIt : g_game.accountStorageMap) {
		if (accountIt.second.empty()) {
			continue;
		}

		DBInsert accountStorageQuery("INSERT INTO `account_storage` (`account_id`, `key`, `value`) VALUES");
		for (const auto& storageIt : accountIt.second) {
			if (!accountStorageQuery.addRow(fmt::format("{:d}, {:d}, {:d}", accountIt.first, storageIt.first, storageIt.second))) {
				return false;
			}
		}

		if (!accountStorageQuery.execute()) {
			return false;
		}
	}

	return transaction.commit();
}

void Game::startDecay(const ItemPtr& item)
{
	if (!item || !item->canDecay()) {
		return;
	}

	ItemDecayState_t decayState = item->getDecaying();
	if (decayState == DECAYING_TRUE) {
		return;
	}

	if (item->getDuration() > 0) {
		item->setDecaying(DECAYING_TRUE);
		toDecayItems.push_front(item);
	} else {
		internalDecayItem(item);
	}
}

void Game::internalDecayItem(const ItemPtr& item)
{
	if (const int32_t decayTo = item->getDecayTo(); decayTo > 0) {
		startDecay(transformItem(item, decayTo));
	} else {
		if (const ReturnValue ret = internalRemoveItem(item); ret != RETURNVALUE_NOERROR) {
			std::cout << "[Debug - Game::internalDecayItem] internalDecayItem failed, error code: " << static_cast<uint32_t>(ret) << ", item id: " << item->getID() << std::endl;
		}
	}
}

void Game::checkDecay()
{
	g_scheduler.addEvent(createSchedulerTask(EVENT_DECAYINTERVAL, [=, this]() { checkDecay(); }));

	const size_t bucket = (lastBucket + 1) % EVENT_DECAY_BUCKETS;

	auto it = decayItems[bucket].begin();
	const auto end = decayItems[bucket].end();
	while (it != end) {
		const auto item = *it;
		if (!item->canDecay()) {
			item->setDecaying(DECAYING_FALSE);
			// ReleaseItem(item);
			it = decayItems[bucket].erase(it);
			continue;
		}

		int32_t duration = item->getDuration();
		const int32_t decreaseTime = std::min<int32_t>(EVENT_DECAYINTERVAL * EVENT_DECAY_BUCKETS, duration);

		duration -= decreaseTime;
		item->decreaseDuration(decreaseTime);

		if (duration <= 0) {
			it = decayItems[bucket].erase(it);
			internalDecayItem(item);
			// ReleaseItem(item);
		} else if (duration < EVENT_DECAYINTERVAL * EVENT_DECAY_BUCKETS) {
			it = decayItems[bucket].erase(it);
			if (const size_t newBucket = (bucket + ((duration + EVENT_DECAYINTERVAL / 2) / 1000)) % EVENT_DECAY_BUCKETS; newBucket == bucket) {
				internalDecayItem(item);
				// ReleaseItem(item);
			} else {
				decayItems[newBucket].push_back(item);
			}
		} else {
			++it;
		}
	}

	lastBucket = bucket;
	cleanup();
}

void Game::checkLight()
{
	g_scheduler.addEvent(createSchedulerTask(EVENT_LIGHTINTERVAL, [=, this]() { checkLight(); }));
	uint8_t previousLightLevel = lightLevel;
	updateWorldLightLevel();
	
	if (previousLightLevel != lightLevel) {
		LightInfo lightInfo = getWorldLightInfo();

		for (const auto& val : players | std::views::values) {
			val->sendWorldLight(lightInfo);
		}
	}
}

void Game::updateWorldLightLevel()
{
	if (getWorldTime() >= GAME_SUNRISE && getWorldTime() <= GAME_DAYTIME) {
		lightLevel = ((GAME_DAYTIME - GAME_SUNRISE) - (GAME_DAYTIME - getWorldTime())) * static_cast<float>(LIGHT_CHANGE_SUNRISE) + LIGHT_NIGHT;
	} else if (getWorldTime() >= GAME_SUNSET && getWorldTime() <= GAME_NIGHTTIME) {
		lightLevel = LIGHT_DAY - ((getWorldTime() - GAME_SUNSET) * static_cast<float>(LIGHT_CHANGE_SUNSET));
	} else if (getWorldTime() >= GAME_NIGHTTIME || getWorldTime() < GAME_SUNRISE) {
		lightLevel = LIGHT_NIGHT;
	} else {
		lightLevel = LIGHT_DAY;
	}
}

void Game::updateWorldTime()
{
	g_scheduler.addEvent(createSchedulerTask(EVENT_WORLDTIMEINTERVAL, [=, this]() { updateWorldTime(); }));
	const time_t osTime = time(nullptr);
	const auto timeInfo = localtime(&osTime);
	worldTime = (timeInfo->tm_sec + (timeInfo->tm_min * 60)) / 2.5f;
}

void Game::shutdown()
{
	std::cout << "Shutting down..." << std::flush;

	g_scheduler.shutdown();
	g_databaseTasks.shutdown();
	g_dispatcher.shutdown();
	g_dispatcher_discord.shutdown();
	map.spawns.clear();
	raids.clear();

	cleanup();

	if (serviceManager) {
		serviceManager->stop();
	}

	ConnectionManager::getInstance().closeAll();

	std::cout << " done!" << std::endl;
}

void Game::cleanup()
{
	//free memory
	for (const auto item : toDecayItems) {
		const uint32_t dur = item->getDuration();
		if (dur >= EVENT_DECAYINTERVAL * EVENT_DECAY_BUCKETS) {
			decayItems[lastBucket].push_back(item);
		} else {
			decayItems[(lastBucket + 1 + dur / 1000) % EVENT_DECAY_BUCKETS].push_back(item);
		}
	}
	toDecayItems.clear();
}

void Game::broadcastMessage(const std::string& text, const MessageClasses type) const
{
	std::cout << "> Broadcasted message: \"" << text << "\"." << std::endl;
	for (const auto& val : players | std::views::values) {
		val->sendTextMessage(type, text);
	}
}

void Game::updateCreatureWalkthrough(const CreatureConstPtr& creature)
{
	//send to clients
	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);

		const PlayerPtr spectatorPlayer = std::static_pointer_cast<Player>(spectator);
		spectatorPlayer->sendCreatureWalkthrough(creature, spectatorPlayer->canWalkthroughEx(creature));
	}
}

void Game::notifySpectators(const CreatureConstPtr& creature)
{
	// send to clients
	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendUpdateTileCreature(creature);
	}
}

void Game::updateCreatureSkull(const CreatureConstPtr& creature)
{
	if (getWorldType() != WORLD_TYPE_PVP) {
		return;
	}

	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendCreatureSkull(creature);
	}
}

void Game::updatePlayerShield(const PlayerPtr& player)
{
	SpectatorVec spectators;
	map.getSpectators(spectators, player->getPosition(), true, true);
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendCreatureShield(player);
	}
}

void Game::updatePlayerHelpers(const PlayerConstPtr& player)
{
	uint32_t creatureId = player->getID();
	uint16_t helpers = player->getHelpers();

	SpectatorVec spectators;
	map.getSpectators(spectators, player->getPosition(), true, true);
	for (const auto spectator : spectators) {
		spectator->getPlayer()->sendCreatureHelpers(creatureId, helpers);
	}
}

void Game::updateCreatureType(const CreaturePtr& creature)
{
	PlayerPtr masterPlayer = nullptr;

	const uint32_t creatureId = creature->getID();
	CreatureType_t creatureType = creature->getType();
	if (creatureType == CREATURETYPE_MONSTER) {
		if (const auto master = creature->getMaster()) {
			masterPlayer = master->getPlayer();
			if (masterPlayer) {
				creatureType = CREATURETYPE_SUMMON_HOSTILE;
			}
		}
	}

	//send to clients
	SpectatorVec spectators;
	map.getSpectators(spectators, creature->getPosition(), true, true);

	if (creatureType == CREATURETYPE_SUMMON_HOSTILE) {
		for (const auto spectator : spectators) {
			const auto player = spectator->getPlayer();
			if (masterPlayer == player) {
				player->sendCreatureType(creatureId, CREATURETYPE_SUMMON_OWN);
			} else {
				player->sendCreatureType(creatureId, creatureType);
			}
		}
	} else {
		for (const auto spectator : spectators) {
			spectator->getPlayer()->sendCreatureType(creatureId, creatureType);
		}
	}
}

void Game::loadMotdNum()
{
	Database& db = Database::getInstance();

	DBResult_ptr result = db.storeQuery("SELECT `value` FROM `server_config` WHERE `config` = 'motd_num'");
	if (result) {
		motdNum = result->getNumber<uint32_t>("value");
	} else {
		db.executeQuery("INSERT INTO `server_config` (`config`, `value`) VALUES ('motd_num', '0')");
	}

	result = db.storeQuery("SELECT `value` FROM `server_config` WHERE `config` = 'motd_hash'");
	if (result) {
		motdHash = result->getString("value");
		if (motdHash != transformToSHA1(g_config.getString(ConfigManager::MOTD))) {
			++motdNum;
		}
	} else {
		db.executeQuery("INSERT INTO `server_config` (`config`, `value`) VALUES ('motd_hash', '')");
	}
}

void Game::saveMotdNum() const
{
	Database& db = Database::getInstance();
	db.executeQuery(fmt::format("UPDATE `server_config` SET `value` = '{:d}' WHERE `config` = 'motd_num'", motdNum));
	db.executeQuery(fmt::format("UPDATE `server_config` SET `value` = '{:s}' WHERE `config` = 'motd_hash'", transformToSHA1(g_config.getString(ConfigManager::MOTD))));
}

void Game::checkPlayersRecord()
{
	const size_t playersOnline = getPlayersOnline();
	if (playersOnline > playersRecord) {
		const uint32_t previousRecord = playersRecord;
		playersRecord = playersOnline;

		for (auto& it : g_globalEvents->getEventMap(GLOBALEVENT_RECORD)) {
			it.second.executeRecord(playersRecord, previousRecord);
		}
		updatePlayersRecord();
	}
}

void Game::updatePlayersRecord() const
{
	Database& db = Database::getInstance();
	db.executeQuery(fmt::format("UPDATE `server_config` SET `value` = '{:d}' WHERE `config` = 'players_record'", playersRecord));
}

void Game::loadPlayersRecord()
{
	Database& db = Database::getInstance();

	if (const DBResult_ptr result = db.storeQuery("SELECT `value` FROM `server_config` WHERE `config` = 'players_record'")) {
		playersRecord = result->getNumber<uint32_t>("value");
	} else {
		db.executeQuery("INSERT INTO `server_config` (`config`, `value`) VALUES ('players_record', '0')");
	}
}

void Game::playerInviteToParty(const uint32_t playerId, const uint32_t invitedId)
{
	if (playerId == invitedId) {
		return;
	}

	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	auto invitedPlayer = getPlayerByID(invitedId);
	if (!invitedPlayer || invitedPlayer->isInviting(player)) {
		return;
	}

	if (invitedPlayer->getParty()) {
		player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} is already in a party.", invitedPlayer->getName()));
		return;
	}

	auto party = player->getParty();
	if (!party) {
		party = new Party(player);
	} else if (party->getLeader() != player) {
		return;
	}

	if (!g_events->eventPartyOnInvite(party, invitedPlayer)) {
		if (party->empty()) {
			player->setParty(nullptr);
			delete party;
		}
		return;
	}

	party->invitePlayer(invitedPlayer);
}

void Game::playerJoinParty(const uint32_t playerId, const uint32_t leaderId)
{
	auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto leader = getPlayerByID(leaderId);
	if (!leader || !leader->isInviting(player)) {
		return;
	}

	const auto party = leader->getParty();
	if (!party || party->getLeader() != leader) {
		return;
	}

	if (player->getParty()) {
		player->sendTextMessage(MESSAGE_INFO_DESCR, "You are already in a party.");
		return;
	}

	party->joinParty(player);
}

void Game::playerRevokePartyInvitation(const uint32_t playerId, const uint32_t invitedId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto party = player->getParty();
	if (!party || party->getLeader() != player) {
		return;
	}

	auto invitedPlayer = getPlayerByID(invitedId);
	if (!invitedPlayer || !player->isInviting(invitedPlayer)) {
		return;
	}

	party->revokeInvitation(invitedPlayer);
}

void Game::playerPassPartyLeadership(const uint32_t playerId, const uint32_t newLeaderId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto party = player->getParty();
	if (!party || party->getLeader() != player) {
		return;
	}

	const auto newLeader = getPlayerByID(newLeaderId);
	if (!newLeader || !player->isPartner(newLeader)) {
		return;
	}

	party->passPartyLeadership(newLeader);
}

void Game::playerLeaveParty(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto party = player->getParty();
	if (!party) {
		return;
	}

	if (player->hasCondition(CONDITION_INFIGHT)) {
		player->sendCancelMessage("You may not leave the party during a fight.");
		return;
	}

	party->leaveParty(player);
}

void Game::playerEnableSharedPartyExperience(const uint32_t playerId, bool sharedExpActive)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	const auto party = player->getParty();
	if (!party) {
		return;
	}

	if (player->hasCondition(CONDITION_INFIGHT) && player->getZone() != ZONE_PROTECTION) {
		player->sendCancelMessage("You cannot enable shared experience during a fight.");
		return;
	}

	party->setSharedExperience(player, sharedExpActive);
}

void Game::sendGuildMotd(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (const auto guild = player->getGuild()) {
		player->sendChannelMessage("Message of the Day", guild->getMotd(), TALKTYPE_CHANNEL_R1, CHANNEL_GUILD);
	}
}

void Game::kickPlayer(const uint32_t playerId, const bool displayEffect)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->kickPlayer(displayEffect);
}

void Game::playerReportRuleViolation(const uint32_t playerId, const std::string& targetName, uint8_t reportType, uint8_t reportReason, const std::string& comment, const std::string& translation)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	g_events->eventPlayerOnReportRuleViolation(player, targetName, reportType, reportReason, comment, translation);
}

void Game::playerReportBug(const uint32_t playerId, const std::string& message, const Position& position, uint8_t category)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	g_events->eventPlayerOnReportBug(player, message, position, category);
}

void Game::playerDebugAssert(const uint32_t playerId, const std::string& assertLine, const std::string& date, const std::string& description, const std::string& comment)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	// TODO: move debug assertions to database
	if (const auto file = fopen("client_assertions.txt", "a")) {
		fprintf(file, "----- %s - %s (%s) -----\n", formatDate(time(nullptr)).c_str(), player->getName().c_str(), convertIPToString(player->getIP()).c_str());
		fprintf(file, "%s\n%s\n%s\n%s\n", assertLine.c_str(), date.c_str(), description.c_str(), comment.c_str());
		fclose(file);
	}
}

void Game::playerLeaveMarket(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	player->setInMarket(false);
}

void Game::playerBrowseMarket(const uint32_t playerId, const uint16_t spriteId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!player->isInMarket()) {
		return;
	}

	const ItemType& it = Item::items.getItemIdByClientId(spriteId);
	if (it.id == 0) {
		return;
	}

	if (it.wareId == 0) {
		return;
	}

	const MarketOfferList& buyOffers = IOMarket::getActiveOffers(MARKETACTION_BUY, it.id);
	const MarketOfferList& sellOffers = IOMarket::getActiveOffers(MARKETACTION_SELL, it.id);
	player->sendMarketBrowseItem(it.id, buyOffers, sellOffers);
	player->sendMarketDetail(it.id);
}

void Game::playerBrowseMarketOwnOffers(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!player->isInMarket()) {
		return;
	}

	const MarketOfferList& buyOffers = IOMarket::getOwnOffers(MARKETACTION_BUY, player->getGUID());
	const MarketOfferList& sellOffers = IOMarket::getOwnOffers(MARKETACTION_SELL, player->getGUID());
	player->sendMarketBrowseOwnOffers(buyOffers, sellOffers);
}

void Game::playerBrowseMarketOwnHistory(const uint32_t playerId)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!player->isInMarket()) {
		return;
	}

	const HistoryMarketOfferList& buyOffers = IOMarket::getOwnHistory(MARKETACTION_BUY, player->getGUID());
	const HistoryMarketOfferList& sellOffers = IOMarket::getOwnHistory(MARKETACTION_SELL, player->getGUID());
	player->sendMarketBrowseOwnHistory(buyOffers, sellOffers);
}

void Game::playerCreateMarketOffer(const uint32_t playerId, uint8_t type, const uint16_t spriteId, const uint16_t amount, const uint32_t price, const bool anonymous)
{
	if (amount == 0 || amount > 64000) {
		return;
	}

	if (price == 0 || price > 999999999) {
		return;
	}

	if (type != MARKETACTION_BUY && type != MARKETACTION_SELL) {
		return;
	}

	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!player->isInMarket()) {
		return;
	}

	if (g_config.getBoolean(ConfigManager::MARKET_PREMIUM) && !player->isPremium()) {
		player->sendMarketLeave();
		return;
	}

	const ItemType& itt = Item::items.getItemIdByClientId(spriteId);
	if (itt.id == 0 || itt.wareId == 0) {
		return;
	}

	const ItemType& it = Item::items.getItemIdByClientId(itt.wareId);
	if (it.id == 0 || it.wareId == 0) {
		return;
	}

	if (!it.stackable && amount > 2000) {
		return;
	}

	const uint32_t maxOfferCount = g_config.getNumber(ConfigManager::MAX_MARKET_OFFERS_AT_A_TIME_PER_PLAYER);
	if (maxOfferCount != 0 && IOMarket::getPlayerOfferCount(player->getGUID()) >= maxOfferCount) {
		return;
	}

	uint64_t fee = (price / 100.) * amount;
	if (fee < 20) {
		fee = 20;
	} else if (fee > 1000) {
		fee = 1000;
	}

	if (type == MARKETACTION_SELL) {
		if (fee > (player->getMoney() + player->bankBalance)) {
			return;
		}

		const auto& itemList = getMarketItemList(it.wareId, amount, player);
		if (itemList.empty()) {
			return;
		}

		if (it.stackable) {
			uint16_t tmpAmount = amount;
			for (const auto item : itemList) {
				const uint16_t removeCount = std::min<uint16_t>(tmpAmount, item->getItemCount());
				tmpAmount -= removeCount;
				internalRemoveItem(item, removeCount);

				if (tmpAmount == 0) {
					break;
				}
			}
		} else {
			for (const auto item : itemList) {
				internalRemoveItem(item);
			}
		}

		const auto debitCash = std::min(player->getMoney(), fee);
		const auto debitBank = fee - debitCash;
		CylinderPtr c_player = player;
		removeMoney(c_player, debitCash);
		player->bankBalance -= debitBank;
	} else {
		uint64_t totalPrice = static_cast<uint64_t>(price) * amount;
		totalPrice += fee;
		if (totalPrice > (player->getMoney() + player->bankBalance)) {
			return;
		}

		const auto debitCash = std::min(player->getMoney(), totalPrice);
		const auto debitBank = totalPrice - debitCash;
		CylinderPtr c_player = player;
		removeMoney(c_player, debitCash);
		player->bankBalance -= debitBank;
	}

	IOMarket::createOffer(player->getGUID(), static_cast<MarketAction_t>(type), it.id, amount, price, anonymous);

	player->sendMarketEnter();
	const MarketOfferList& buyOffers = IOMarket::getActiveOffers(MARKETACTION_BUY, it.id);
	const MarketOfferList& sellOffers = IOMarket::getActiveOffers(MARKETACTION_SELL, it.id);
	player->sendMarketBrowseItem(it.id, buyOffers, sellOffers);
}

void Game::playerCancelMarketOffer(const uint32_t playerId, const uint32_t timestamp, const uint16_t counter)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!player->isInMarket()) {
		return;
	}

	MarketOfferEx offer = IOMarket::getOfferByCounter(timestamp, counter);
	if (offer.id == 0 || offer.playerId != player->getGUID()) {
		return;
	}

	if (offer.type == MARKETACTION_BUY) {
		player->bankBalance += static_cast<uint64_t>(offer.price) * offer.amount;
		player->sendMarketEnter();
	} else {
		const ItemType& it = Item::items[offer.itemId];
		if (it.id == 0) {
			return;
		}

		if (it.stackable) {
			uint16_t tmpAmount = offer.amount;
			while (tmpAmount > 0) {
				int32_t stackCount = std::min<int32_t>(100, tmpAmount);
				auto item = Item::CreateItem(it.id, stackCount);
				if (CylinderPtr inbox = player->getInbox(); internalAddItem(inbox, item, INDEX_WHEREEVER, FLAG_NOLIMIT) != RETURNVALUE_NOERROR) {
					item.reset();
					break;
				}

				tmpAmount -= stackCount;
			}
		} else {
			int32_t subType;
			if (it.charges != 0) {
				subType = it.charges;
			} else {
				subType = -1;
			}

			for (uint16_t i = 0; i < offer.amount; ++i) {
				auto item = Item::CreateItem(it.id, subType);
				if (CylinderPtr inbox = player->getInbox(); internalAddItem(inbox, item, INDEX_WHEREEVER, FLAG_NOLIMIT) != RETURNVALUE_NOERROR) {
					item.reset();
					break;
				}
			}
		}
	}

	IOMarket::moveOfferToHistory(offer.id, OFFERSTATE_CANCELLED);
	offer.amount = 0;
	offer.timestamp += g_config.getNumber(ConfigManager::MARKET_OFFER_DURATION);
	player->sendMarketCancelOffer(offer);
	player->sendMarketEnter();
}

void Game::playerAcceptMarketOffer(const uint32_t playerId, const uint32_t timestamp, const uint16_t counter, const uint16_t amount)
{
	if (amount == 0 || amount > 64000) {
		return;
	}

	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!player->isInMarket()) {
		return;
	}

	MarketOfferEx offer = IOMarket::getOfferByCounter(timestamp, counter);
	if (offer.id == 0) {
		return;
	}

	uint32_t offerAccountId = IOLoginData::getAccountIdByPlayerId(offer.playerId);
	if (offerAccountId == player->getAccount()) {
		return;
	}

	if (amount > offer.amount) {
		return;
	}

	const ItemType& it = Item::items[offer.itemId];
	if (it.id == 0) {
		return;
	}

	uint64_t totalPrice = static_cast<uint64_t>(offer.price) * amount;

	if (offer.type == MARKETACTION_BUY) {
		const auto& itemList = getMarketItemList(it.wareId, amount, player);
		if (itemList.empty()) {
			return;
		}

		auto buyerPlayer = getPlayerByGUID(offer.playerId);
		if (!buyerPlayer) {
			buyerPlayer = std::make_shared<Player>(nullptr);
			if (!IOLoginData::loadPlayerById(buyerPlayer, offer.playerId)) {
				buyerPlayer.reset();
				return;
			}
		}

		if (it.stackable) {
			uint16_t tmpAmount = amount;
			for (const auto item : itemList) {
				const uint16_t removeCount = std::min<uint16_t>(tmpAmount, item->getItemCount());
				tmpAmount -= removeCount;
				internalRemoveItem(item, removeCount);

				if (tmpAmount == 0) {
					break;
				}
			}
		} else {
			for (const auto item : itemList) {
				internalRemoveItem(item);
			}
		}

		player->bankBalance += totalPrice;

		if (it.stackable) {
			uint16_t tmpAmount = amount;
			while (tmpAmount > 0) {
				uint16_t stackCount = std::min<uint16_t>(100, tmpAmount);
				auto item = Item::CreateItem(it.id, stackCount);
				CylinderPtr inbox = player->getInbox();
				if (internalAddItem(inbox, item, INDEX_WHEREEVER, FLAG_NOLIMIT) != RETURNVALUE_NOERROR) {
					item.reset();
					break;
				}

				tmpAmount -= stackCount;
			}
		} else {
			int32_t subType;
			if (it.charges != 0) {
				subType = it.charges;
			} else {
				subType = -1;
			}

			for (uint16_t i = 0; i < amount; ++i) {
				auto item = Item::CreateItem(it.id, subType);
				CylinderPtr inbox = player->getInbox();
				if (internalAddItem(inbox, item, INDEX_WHEREEVER, FLAG_NOLIMIT) != RETURNVALUE_NOERROR) {
					item.reset();
					break;
				}
			}
		}

		if (buyerPlayer->isOffline()) {
			IOLoginData::savePlayer(buyerPlayer);
			buyerPlayer.reset();
		} else {
			buyerPlayer->onReceiveMail();
		}
	} else {
		if (totalPrice > (player->getMoney() + player->bankBalance)) {
			return;
		}

		const auto debitCash = std::min(player->getMoney(), totalPrice);
		const auto debitBank = totalPrice - debitCash;
		CylinderPtr coinholder = player;
		removeMoney(coinholder, debitCash);
		player->bankBalance -= debitBank;

		if (it.stackable) {
			uint16_t tmpAmount = amount;
			while (tmpAmount > 0) {
				uint16_t stackCount = std::min<uint16_t>(100, tmpAmount);
				auto item = Item::CreateItem(it.id, stackCount);
				CylinderPtr inbox = player->getInbox();
				if (internalAddItem(inbox, item, INDEX_WHEREEVER, FLAG_NOLIMIT) != RETURNVALUE_NOERROR) {
					item.reset();
					break;
				}

				tmpAmount -= stackCount;
			}
		} else {
			int32_t subType;
			if (it.charges != 0) {
				subType = it.charges;
			} else {
				subType = -1;
			}

			for (uint16_t i = 0; i < amount; ++i) {
				auto item = Item::CreateItem(it.id, subType);
				CylinderPtr inbox = player->getInbox();
				if (internalAddItem(inbox, item, INDEX_WHEREEVER, FLAG_NOLIMIT) != RETURNVALUE_NOERROR) {
					item.reset();
					break;
				}
			}
		}

		if (const auto sellerPlayer = getPlayerByGUID(offer.playerId)) {
			sellerPlayer->bankBalance += totalPrice;
		} else {
			IOLoginData::increaseBankBalance(offer.playerId, totalPrice);
		}

		player->onReceiveMail();
	}

	const int32_t marketOfferDuration = g_config.getNumber(ConfigManager::MARKET_OFFER_DURATION);

	IOMarket::appendHistory(player->getGUID(), (offer.type == MARKETACTION_BUY ? MARKETACTION_SELL : MARKETACTION_BUY), offer.itemId, amount, offer.price, offer.timestamp + marketOfferDuration, OFFERSTATE_ACCEPTEDEX);

	IOMarket::appendHistory(offer.playerId, offer.type, offer.itemId, amount, offer.price, offer.timestamp + marketOfferDuration, OFFERSTATE_ACCEPTED);

	offer.amount -= amount;

	if (offer.amount == 0) {
		IOMarket::deleteOffer(offer.id);
	} else {
		IOMarket::acceptOffer(offer.id, amount);
	}

	player->sendMarketEnter();
	offer.timestamp += marketOfferDuration;
	player->sendMarketAcceptOffer(offer);
}

void Game::parsePlayerExtendedOpcode(const uint32_t playerId, const uint8_t opcode, const std::string& buffer)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	for (const auto creatureEvent : player->getCreatureEvents(CREATURE_EVENT_EXTENDED_OPCODE)) {
		creatureEvent->executeExtendedOpcode(player, opcode, buffer);
	}
}

std::vector<ItemPtr> Game::getMarketItemList(const uint16_t wareId, const uint16_t sufficientCount, const PlayerConstPtr& player)
{
	uint16_t count = 0;
	std::list<ContainerPtr> containers{ player->getInbox() };

	for (const auto& val : player->depotChests | std::views::values) {
		if (!val->empty()) {
			containers.push_front(val);
		}
	}

	std::vector<ItemPtr> itemList;
	do {
		auto container = containers.front();
		containers.pop_front();

		for (const auto item : container->getItemList()) {
			const auto containerItem = item->getContainer();
			if (containerItem && !containerItem->empty()) {
				containers.push_back(containerItem);
				continue;
			}

			const ItemType& itemType = Item::items[item->getID()];
			if (itemType.wareId != wareId) {
				continue;
			}

			if (containerItem && (!itemType.isContainer() || containerItem->capacity() != itemType.maxItems)) {
				continue;
			}

			if (!item->hasMarketAttributes()) {
				continue;
			}

			itemList.push_back(item);

			count += Item::countByType(item, -1);
			if (count >= sufficientCount) {
				return itemList;
			}
		}
	} while (!containers.empty());
	return {};
}

void Game::forceAddCondition(const uint32_t creatureId, Condition* condition)
{
	const auto creature = getCreatureByID(creatureId);
	if (!creature) {
		delete condition;
		return;
	}

	creature->addCondition(condition, true);
}

void Game::forceRemoveCondition(const uint32_t creatureId, ConditionType_t type)
{
	const auto creature = getCreatureByID(creatureId);
	if (!creature) {
		return;
	}

	creature->removeCondition(type, true);
}

void Game::sendOfflineTrainingDialog(const PlayerPtr& player) const
{
	if (!player) {
		return;
	}

	if (!player->hasModalWindowOpen(offlineTrainingWindow.id)) {
		player->sendModalWindow(offlineTrainingWindow);
	}
}

void Game::playerAnswerModalWindow(const uint32_t playerId, const uint32_t modalWindowId, const uint8_t button, const uint8_t choice)
{
	const auto player = getPlayerByID(playerId);
	if (!player) {
		return;
	}

	if (!player->hasModalWindowOpen(modalWindowId)) {
		return;
	}

	if (player->isAccountManager()) 
	{
		if (player->getAccount() == 1) 
		{
			g_game.onAccountManagerInput(player, modalWindowId, button, choice);
		}
		g_game.onPrivateAccountManagerInput(player, modalWindowId, button, choice);
		return;
	}

	player->onModalWindowHandled(modalWindowId);

	// offline training, hard-coded
	if (modalWindowId == std::numeric_limits<uint32_t>::max()) {
		if (button == offlineTrainingWindow.defaultEnterButton) {
			if (choice == SKILL_SWORD || choice == SKILL_AXE || choice == SKILL_CLUB || choice == SKILL_DISTANCE || choice == SKILL_MAGLEVEL) {
				const auto bedItem = player->getBedItem();
				if (bedItem && bedItem->sleep(player)) {
					player->setOfflineTrainingSkill(choice);
					return;
				}
			}
		} else {
			player->sendTextMessage(MESSAGE_EVENT_ADVANCE, "Offline training aborted.");
		}

		player->setBedItem(nullptr);
	} else {
		for (auto creatureEvent : player->getCreatureEvents(CREATURE_EVENT_MODALWINDOW)) {
			creatureEvent->executeModalWindow(player, modalWindowId, button, choice);
		}
	}
}

void Game::addPlayer(PlayerPtr player)
{
	const std::string& lowercase_name = asLowerCaseString(player->getName());
	mappedPlayerNames[lowercase_name] = player;
	mappedPlayerGuids[player->getGUID()] = player;
	wildcardTree.insert(lowercase_name);
	players[player->getID()] = player;
}

void Game::removePlayer(const PlayerPtr& player)
{
	const std::string& lowercase_name = asLowerCaseString(player->getName());
	mappedPlayerNames.erase(lowercase_name);
	mappedPlayerGuids.erase(player->getGUID());
	wildcardTree.remove(lowercase_name);
	players.erase(player->getID());
}

void Game::addNpc(const NpcPtr& npc)
{
	npcs[npc->getID()] = npc;
}

void Game::removeNpc(const NpcPtr& npc)
{
	npcs.erase(npc->getID());
}

void Game::addMonster(MonsterPtr monster)
{
	monsters[monster->getID()] = monster;
}

void Game::removeMonster(const MonsterPtr& monster)
{
	monsters.erase(monster->getID());
}

Guild* Game::getGuild(const uint32_t id) const
{
	const auto it = guilds.find(id);
	if (it == guilds.end()) {
		return nullptr;
	}
	return it->second;
}

void Game::addGuild(Guild* guild)
{
	guilds[guild->getId()] = guild;
}

void Game::removeGuild(uint32_t guildId)
{
	guilds.erase(guildId);
}

void Game::internalRemoveItems(const std::vector<ItemPtr>& itemList, uint32_t amount, const bool stackable)
{
	if (stackable) {
		for (const auto item : itemList) {
			if (item->getItemCount() > amount) {
				internalRemoveItem(item, amount);
				break;
			} else {
				amount -= item->getItemCount();
				internalRemoveItem(item);
			}
		}
	} else {
		for (const auto item : itemList) {
			internalRemoveItem(item);
		}
	}
}

BedItemPtr Game::getBedBySleeper(const uint32_t guid) const
{
	const auto it = bedSleepersMap.find(guid);
	if (it == bedSleepersMap.end()) {
		return nullptr;
	}
	return it->second;
}

void Game::setBedSleeper(const BedItemPtr& bed, const uint32_t guid)
{
	bedSleepersMap[guid] = bed;
}

void Game::removeBedSleeper(const uint32_t guid)
{
	if (auto it = bedSleepersMap.find(guid); it != bedSleepersMap.end()) {
		bedSleepersMap.erase(it);
	}
}

ItemPtr Game::getUniqueItem(const uint16_t uniqueId)
{
	const auto it = uniqueItems.find(uniqueId);
	if (it == uniqueItems.end()) {
		return nullptr;
	}
	return it->second;
}

bool Game::addUniqueItem(const uint16_t uniqueId, const ItemPtr& item)
{
	const auto result = uniqueItems.emplace(uniqueId, item);
	if (!result.second) {
		std::cout << "Duplicate unique id: " << uniqueId << std::endl;
	}
	return result.second;
}

void Game::removeUniqueItem(const uint16_t uniqueId)
{
	if (auto it = uniqueItems.find(uniqueId); it != uniqueItems.end()) {
		uniqueItems.erase(it);
	}
}

bool Game::reload(const ReloadTypes_t reloadType)
{
	switch (reloadType) {
		case RELOAD_TYPE_ACTIONS: return g_actions->reload();
		case RELOAD_TYPE_AUGMENTS: {
			g_augments->reload();
			return true;
	   }
		case RELOAD_TYPE_CHAT: return g_chat->load();
		case RELOAD_TYPE_CONFIG: return g_config.reload();
		case RELOAD_TYPE_CREATURESCRIPTS: {
			g_creatureEvents->reload();
			g_creatureEvents->removeInvalidEvents();
			return true;
		}
		case RELOAD_TYPE_EVENTS: return g_events->load();
		case RELOAD_TYPE_GLOBALEVENTS: return g_globalEvents->reload();
		case RELOAD_TYPE_ITEMS: return Item::items.reload();
		case RELOAD_TYPE_MONSTERS: return g_monsters.reload();
		case RELOAD_TYPE_MOUNTS: return mounts.reload();
		case RELOAD_TYPE_MOVEMENTS: return g_moveEvents->reload();
		case RELOAD_TYPE_NPCS: {
			Npcs::reload();
			return true;
		}

		case RELOAD_TYPE_QUESTS: return quests.reload();
		case RELOAD_TYPE_RAIDS: return raids.reload() && raids.startup();

		case RELOAD_TYPE_SPELLS: {
			if (!g_spells->reload()) {
				std::cout << "[Error - Game::reload] Failed to reload spells." << std::endl;
				std::terminate();
			} else if (!g_monsters.reload()) {
				std::cout << "[Error - Game::reload] Failed to reload monsters." << std::endl;
				std::terminate();
			}
			return true;
		}

		case RELOAD_TYPE_TALKACTIONS: return g_talkActions->reload();

		case RELOAD_TYPE_WEAPONS: {
			bool results = g_weapons->reload();
			g_weapons->loadDefaults();
			return results;
		}

		case RELOAD_TYPE_SCRIPTS: {
			// commented out stuff is TODO, once we approach further in revscriptsys
			g_actions->clear(true);
			g_creatureEvents->clear(true);
			g_moveEvents->clear(true);
			g_talkActions->clear(true);
			g_globalEvents->clear(true);
			g_weapons->clear(true);
			g_weapons->loadDefaults();
			g_spells->clear(true);
			g_scripts->loadScripts("scripts", false, true);
			g_creatureEvents->removeInvalidEvents();
			g_augments->reload();
			/*
			Npcs::reload();
			raids.reload() && raids.startup();
			Item::items.reload();
			quests.reload();
			mounts.reload();
			g_config.reload();
			g_events->load();
			g_chat->load();
			*/
			return true;
		}

		default: {
			if (!g_spells->reload()) {
				std::cout << "[Error - Game::reload] Failed to reload spells." << std::endl;
				std::terminate();
			} else if (!g_monsters.reload()) {
				std::cout << "[Error - Game::reload] Failed to reload monsters." << std::endl;
				std::terminate();
			}

			g_actions->reload();
			g_config.reload();
			g_creatureEvents->reload();
			g_monsters.reload();
			g_moveEvents->reload();
			Npcs::reload();
			raids.reload() && raids.startup();
			g_talkActions->reload();
			Item::items.reload();
			g_weapons->reload();
			g_weapons->clear(true);
			g_weapons->loadDefaults();
			quests.reload();
			mounts.reload();
			g_globalEvents->reload();
			g_events->load();
			g_chat->load();
			g_actions->clear(true);
			g_creatureEvents->clear(true);
			g_moveEvents->clear(true);
			g_talkActions->clear(true);
			g_globalEvents->clear(true);
			g_spells->clear(true);
			g_scripts->loadScripts("scripts", false, true);
			g_creatureEvents->removeInvalidEvents();
			return true;
		}
	}
	return true;
}

void Game::resetDamageTracking(const uint32_t monsterId)
{
	rewardBossTracking.erase(monsterId);
}
