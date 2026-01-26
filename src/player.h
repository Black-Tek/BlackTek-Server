// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_PLAYER_H
#define FS_PLAYER_H

#include "creature.h"
#include "container.h"
#include "cylinder.h"
#include "outfit.h"
#include "enums.h"
#include "vocation.h"
#include "protocolgame.h"
#include "party.h"
#include "inbox.h"
#include "depotchest.h"
#include "depotlocker.h"
#include "guild.h"
#include "groups.h"
#include "town.h"
#include "mounts.h"
#include "storeinbox.h"
#include "rewardchest.h"
#include "augments.h"
#include "accountmanager.h"

#include <bitset>
#include <optional>
#include <gtl/phmap.hpp>
#include <gtl/btree.hpp>

class House;
class NetworkMessage;
class Weapon;
class ProtocolGame;
class Npc;
class SchedulerTask;
class Bed;
class Guild;

constexpr uint16_t MaximumStamina = 2520;

enum skillsid_t {
	SKILLVALUE_LEVEL = 0,
	SKILLVALUE_TRIES = 1,
	SKILLVALUE_PERCENT = 2,
};

enum fightMode_t : uint8_t {
	FIGHTMODE_ATTACK = 1,
	FIGHTMODE_BALANCED = 2,
	FIGHTMODE_DEFENSE = 3,
};

enum pvpMode_t : uint8_t {
	PVP_MODE_DOVE = 0,
	PVP_MODE_WHITE_HAND = 1,
	PVP_MODE_YELLOW_HAND = 2,
	PVP_MODE_RED_FIST = 3,
};

enum tradestate_t : uint8_t {
	TRADE_NONE,
	TRADE_INITIATED,
	TRADE_ACCEPT,
	TRADE_ACKNOWLEDGE,
	TRADE_TRANSFER,
};

static constexpr SlotPositionBits getPositionForSlot(slots_t constSlot) {
	switch (constSlot) {
	case CONST_SLOT_HEAD:
		return SLOTP_HEAD;
	case CONST_SLOT_NECKLACE:
		return SLOTP_NECKLACE;
	case CONST_SLOT_BACKPACK:
		return SLOTP_BACKPACK;
	case CONST_SLOT_ARMOR:
		return SLOTP_ARMOR;
	case CONST_SLOT_RIGHT:
		return SLOTP_RIGHT;
	case CONST_SLOT_LEFT:
		return SLOTP_LEFT;
	case CONST_SLOT_LEGS:
		return SLOTP_LEGS;
	case CONST_SLOT_FEET:
		return SLOTP_FEET;
	case CONST_SLOT_AMMO:
		return SLOTP_AMMO;
	case CONST_SLOT_RING:
		return SLOTP_RING;
	default: throw std::invalid_argument("Invalid ConstSlot value");
	}
}

struct VIPEntry {
	VIPEntry(uint32_t guid, std::string_view name, std::string_view description, uint32_t icon, bool notify) : guid{ guid }, name{ name }, description{ description }, icon{ icon }, notify{ notify } {}

	uint32_t guid;
	std::string name;
	std::string description;
	uint32_t icon;
	bool notify;
};

struct OpenContainer {
	ContainerPtr container;
	uint16_t index;
};

struct OutfitEntry {
	constexpr OutfitEntry(uint16_t lookType, uint8_t addons) : lookType(lookType), addons(addons) {}

	uint16_t lookType;
	uint8_t addons;
};

static constexpr int16_t MINIMUM_SKILL_LEVEL = 10;

struct Skill {
	uint64_t tries = 0;
	uint16_t level = MINIMUM_SKILL_LEVEL;
	uint8_t percent = 0;
};

using MuteCountMap = gtl::btree_map<uint32_t, uint32_t>;

static constexpr int32_t PLAYER_MAX_SPEED = 1500;
static constexpr int32_t PLAYER_MIN_SPEED = 10;
static constexpr int32_t NOTIFY_DEPOT_BOX_RANGE = 1;

class Player final : public Creature, public Cylinder
{
	public:
		explicit Player(ProtocolGame_ptr p);
		
		static std::shared_ptr<Player> makePlayer(ProtocolGame_ptr p) {
			auto player = std::shared_ptr<Player>(new Player(p));
			player->storeInbox->setParent(player);
			return player;
		}

		~Player() override;

		// non-copyable
		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		PlayerPtr getPlayer() override {
			return static_shared_this<Player>();
		}
	
		PlayerConstPtr getPlayer() const override {
			return static_shared_this<const Player>();
		}

		void setID() override {
			if (id == 0) {
				id = playerAutoID++;
			}
		}

		static MuteCountMap muteCountMap;

		const std::string& getRegisteredName() const override {
			return getName();
		}

		const std::string& getName() const override {
			return name;
		}
	
		void setName(const std::string_view name) {
			this->name = name; 
		}
	
		const std::string& getNameDescription() const override {
			return name;
		}
	
		std::string getDescription(int32_t lookDistance) override;

		CreatureType_t getType(CreaturePtr caller = nullptr) const override {
			// Todo : create more enums for creature types being relative to caller
			// even for players and npc's, this basic expansion of possibilities
			// applied on this core function would allow vast customization opportunities down the road
			return CREATURETYPE_PLAYER;
		}

		uint8_t getCurrentMount() const;
		void setCurrentMount(uint8_t mountId);
	
		bool isMounted() const {
			return defaultOutfit.lookMount != 0;
		}
	
		bool toggleMount(bool mount);
		bool tameMount(uint8_t mountId);
		bool untameMount(uint8_t mountId);
		bool hasMount(const Mount* mount) const;
		void dismount();
		bool isAccountManager() const { return guid == 1 or name == "Account Manager"; }
		inline bool isPlayer() const override { return true; }
		inline bool isMonster() const override { return false; }
		inline bool isNpc() const override { return false; }

		void sendFYIBox(const std::string& message) const
		{
			if (client) {
				client->sendFYIBox(message);
			}
		}

		void setGUID(const uint32_t guid) {
			this->guid = guid;
		}
	
		uint32_t getGUID() const {
			return guid;
		}
	
		bool canSeeInvisibility() const override {
			return hasFlag(PlayerFlag_CanSenseInvisibility) || group->access;
		}

		void removeList() override;
		void addList() override;
		void kickPlayer(bool displayEffect);

		static uint64_t getExpForLevel(const uint64_t lv) {
			return (((lv - 6ULL) * lv + 17ULL) * lv - 12ULL) / 6ULL * 100ULL;
		}

		uint16_t getStaminaMinutes() const {
			return staminaMinutes;
		}

		bool addOfflineTrainingTries(skills_t skill, uint64_t tries);

		void addOfflineTrainingTime(int32_t addTime) {
			offlineTrainingTime = std::min<int32_t>(12 * 3600 * 1000, offlineTrainingTime + addTime);
		}
	
		void removeOfflineTrainingTime(int32_t removeTime) {
			offlineTrainingTime = std::max<int32_t>(0, offlineTrainingTime - removeTime);
		}
	
		int32_t getOfflineTrainingTime() const {
			return offlineTrainingTime;
		}

		int32_t getOfflineTrainingSkill() const {
			return offlineTrainingSkill;
		}
	
		void setOfflineTrainingSkill(int32_t skill) {
			offlineTrainingSkill = skill;
		}

		uint64_t getBankBalance() const {
			return bankBalance;
		}
	
		void setBankBalance(uint64_t balance) {
			bankBalance = balance;
		}

		Guild_ptr getGuild() const {
			return guild;
		}
	
		void setGuild(Guild_ptr guild);

		GuildRank_ptr getGuildRank() const {
			return guildRank;
		}
	
		void setGuildRank(const GuildRank_ptr& newGuildRank) {
			guildRank = newGuildRank;
		}

		const std::string& getGuildNick() const {
			return guildNick;
		}
	
		void setGuildNick(const std::string& nick) {
			guildNick = nick;
		}

		void setLastWalkthroughAttempt(int64_t walkthroughAttempt) {
			lastWalkthroughAttempt = walkthroughAttempt;
		}
	
		void setLastWalkthroughPosition(Position walkthroughPosition) {
			lastWalkthroughPosition = walkthroughPosition;
		}

		InboxPtr getInbox() const {
			return inbox;
		}

		StoreInboxPtr getStoreInbox() const {
			return storeInbox;
		}

		uint16_t getClientIcons() const;

		Vocation* getVocation() const {
			return vocation;
		}

		OperatingSystem_t getOperatingSystem() const {
			return operatingSystem;
		}
	
		void setOperatingSystem(OperatingSystem_t clientos) {
			operatingSystem = clientos;
		}

		uint16_t getProtocolVersion() const {
			if (!client) {
				return 0;
			}

			return client->getVersion();
		}

		bool hasSecureMode() const {
			return secureMode;
		}

		void setParty(uint32_t party_id) 
		{
			party = party_id;
		}

		void setParty(Party& pt)
		{
			party = pt.getId();
		}
	
		uint32_t getPartyId() const
		{
			return party;
		}

		PartyPtr getParty()  const
		{
			if (not party == 0)
				return Party::get(party);
			return nullptr;
		}
	
		PartyShields_t getPartyShield(const PlayerConstPtr& player) const;
		bool isInviting(const PlayerConstPtr& player) const;
		bool isPartner(const PlayerConstPtr& player) const;
		void sendPlayerPartyIcons(const PlayerPtr& player) const;
		bool addPartyInvitation(PartyPtr party); // todo switch to ids
		void removePartyInvitation(PartyPtr party);
		void clearPartyInvitations();

		GuildEmblems_t getGuildEmblem(const PlayerConstPtr& player) const;
		bool isGuildMate(const PlayerConstPtr player) const;
		bool isGuildWarEnemy(const PlayerConstPtr player, bool alliesAsEnemies) const;
		bool isInWar(const PlayerConstPtr player) const;

		uint64_t getSpentMana() const {
			return manaSpent;
		}

		bool hasFlag(PlayerFlags value) const {
			return (group->flags & value) != 0;
		}

		BedItemPtr getBedItem() {
			return bedItem;
		}
	
		void setBedItem(const BedItemPtr& b) {
			bedItem = b;
		}

		void addBlessing(uint8_t blessing) {
			blessings.set(blessing);
		}
	
		void removeBlessing(uint8_t blessing) {
			blessings.reset(blessing);
		}
	
		bool hasBlessing(uint8_t blessing) const {
			return blessings.test(blessing);
		}

		bool isOffline() const {
			return (getID() == 0);
		}
	
		void disconnect() const
		{
			if (client) {
				client->disconnect();
			}
		}
	
		uint32_t getIP() const;

		void addContainer(uint8_t cid, const ContainerPtr& container);
		void closeContainer(uint8_t cid);
		void setContainerIndex(uint8_t cid, uint16_t index);

		ContainerPtr getContainerByID(uint8_t cid);
		int8_t getContainerID(const ContainerConstPtr& container) const;
        uint16_t getContainerIndex(uint8_t cid) const;

        void autoOpenContainers();

		bool canOpenCorpse(uint32_t ownerId) const;

		void addStorageValue(const uint32_t key, const int32_t value, const bool isLogin = false);
		bool getStorageValue(const uint32_t key, int32_t& value) const;
		void genReservedStorageRange();

		void setGroup(Group* newGroup) {
			group = newGroup;
		}
	
		Group* getGroup() const {
			return group;
		}

		void setInMarket(bool value) {
			inMarket = value;
		}
	
		bool isInMarket() const {
			return inMarket;
		}
	
		int32_t getIdleTime() const {
			return idleTime;
		}

		void resetIdleTime() {
			idleTime = 0;
		}

		bool isInGhostMode() const override {
			return ghostMode;
		}
	
		bool canSeeGhostMode(const CreatureConstPtr& creature) const override;
	
		void switchGhostMode() {
			ghostMode = !ghostMode;
		}

		uint32_t getAccount() const {
			return accountNumber;
		}
	
		AccountType_t getAccountType() const {
			return accountType;
		}
	
		uint32_t getLevel() const {
			return level;
		}
	
		uint8_t getLevelPercent() const {
			return levelPercent;
		}
	
		uint32_t getMagicLevel() const {
			return std::max<int32_t>(0, magLevel + varStats[STAT_MAGICPOINTS]);
		}
	
		uint32_t getBaseMagicLevel() const {
			return magLevel;
		}
	
		uint8_t getMagicLevelPercent() const {
			return magLevelPercent;
		}
	
		uint8_t getSoul() const {
			return soul;
		}
	
		bool isAccessPlayer() const {
			return group->access;
		}
	
		bool isPremium() const;
		void setPremiumTime(time_t premiumEndsAt);

		uint16_t getHelpers() const;

		bool setVocation(uint16_t vocId);
	
		uint16_t getVocationId() const {
			return vocation->getId();
		}

		PlayerSex_t getSex() const {
			return sex;
		}
	
		void setSex(PlayerSex_t);
	
		uint64_t getExperience() const {
			return experience;
		}

		time_t getLastLoginSaved() const {
			return lastLoginSaved;
		}

		time_t getLastLogout() const {
			return lastLogout;
		}

		const Position& getLoginPosition() const {
			return loginPosition;
		}
	
		const Position& getTemplePosition() const {
			return town->getTemplePosition();
		}
	
		Town* getTown() const {
			return town;
		}
	
		void setTown(Town* town) {
			this->town = town;
		}

		void clearModalWindows();
		bool hasModalWindowOpen(uint32_t modalWindowId) const;
		void onModalWindowHandled(uint32_t modalWindowId);

		bool isPushable() const override;
		uint32_t isMuted() const;
		void addMessageBuffer();
		void removeMessageBuffer();

		bool removeItemOfType(uint16_t itemId, uint32_t amount, int32_t subType, bool ignoreEquipped = false) const;

		uint32_t getCapacity() const {
			if (hasFlag(PlayerFlag_CannotPickupItem)) {
				return 0;
			} else if (hasFlag(PlayerFlag_HasInfiniteCapacity)) {
				return std::numeric_limits<uint32_t>::max();
			}
			return capacity;
		}

		uint32_t getFreeCapacity() const {
			if (hasFlag(PlayerFlag_CannotPickupItem)) {
				return 0;
			} else if (hasFlag(PlayerFlag_HasInfiniteCapacity)) {
				return std::numeric_limits<uint32_t>::max();
			} else {
				return std::max<int32_t>(0, capacity - inventoryWeight);
			}
		}

		int32_t getMaxHealth() const override {
			return std::max<int32_t>(1, healthMax + varStats[STAT_MAXHITPOINTS]);
		}
	
		uint32_t getMana() const {
			return mana;
		}
	
		uint32_t getMaxMana() const {
			return std::max<int32_t>(0, manaMax + varStats[STAT_MAXMANAPOINTS]);
		}

		ItemPtr getInventoryItem(slots_t slot) const;
		ItemPtr getInventoryItem(uint32_t slot) const;

		ItemPtr (&getInventory())[CONST_SLOT_LAST + 1] 
		{
			return inventory;
		}

		static bool isInventorySlot(slots_t slot);

		bool isItemAbilityEnabled(slots_t slot) const {
			return inventoryAbilities[slot];
		}
	
		void setItemAbility(slots_t slot, bool enabled) {
			inventoryAbilities[slot] = enabled;
		}

		void setVarSkill(skills_t skill, int32_t modifier) {
			varSkills[skill] += modifier;
		}

		void setVarSpecialSkill(SpecialSkills_t skill, int32_t modifier) {
			varSpecialSkills[skill] += modifier;
		}

		void setVarStats(stats_t stat, int32_t modifier);
		int32_t getDefaultStats(stats_t stat) const;

		void addConditionSuppressions(uint32_t conditions);
		void removeConditionSuppressions(uint32_t conditions);

		DepotChestPtr getDepotChest(uint32_t depotId, bool autoCreate);
		DepotLockerPtr& getDepotLocker();
		uint32_t getDepotItemCount();
		RewardChestPtr& getRewardChest();
		void onReceiveMail() const;
		bool isNearDepotBox() const;

		bool canSee(const Position& pos) const override;
		bool canSeeCreature(const CreatureConstPtr& creature) const override;

		bool canWalkthrough(const CreatureConstPtr& creature) const;
		bool canWalkthroughEx(const CreatureConstPtr& creature) const;

		RaceType_t getRace() const override {
			return RACE_BLOOD;
		}

		uint64_t getMoney() const;

		//safe-trade functions
		void setTradeState(tradestate_t state) {
			tradeState = state;
		}
	
		tradestate_t getTradeState() const {
			return tradeState;
		}
	
		ItemPtr getTradeItem() {
			return tradeItem;
		}

		//shop functions
		void setShopOwner(const NpcPtr& owner, int32_t onBuy, int32_t onSell) {
			shopOwner = owner;
			purchaseCallback = onBuy;
			saleCallback = onSell;
		}

		NpcPtr getShopOwner(int32_t& onBuy, int32_t& onSell) {
			onBuy = purchaseCallback;
			onSell = saleCallback;
			return shopOwner;
		}

		NpcPtr getShopOwner(int32_t& onBuy, int32_t& onSell) const {
			onBuy = purchaseCallback;
			onSell = saleCallback;
			return shopOwner;
		}

		//V.I.P. functions
		void notifyStatusChange(const PlayerPtr& loginPlayer, VipStatus_t status);
		bool removeVIP(uint32_t vipGuid);
		bool addVIP(uint32_t vipGuid, const std::string& vipName, VipStatus_t status);
		bool addVIPInternal(uint32_t vipGuid);
		bool editVIP(uint32_t vipGuid, const std::string& description, uint32_t icon, bool notify);

		//follow functions
		bool setFollowCreature(const CreaturePtr& creature) override;
		void goToFollowCreature() override;

		//follow events
		void onFollowCreature(const CreatureConstPtr& creature) override;

		//walk events
		void onWalk(Direction& dir) override;
		void onWalkAborted() override;
		void onWalkComplete() override;

		void stopWalk();
		void openShopWindow(const NpcPtr& npc, const std::list<ShopInfo>& shop);
		bool closeShopWindow(bool sendCloseShopWindow = true);
		bool updateSaleShopList(const ItemConstPtr& item);
		bool hasShopItemForSale(uint32_t itemId, uint8_t subType) const;

		bool isWearingImbuedItem() const {
			return hasImbuedItemEquipped;
		}

		void checkForImbuedEquipment() {
			bool found = false;
			for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot) {
				ItemPtr item = getInventoryItem(static_cast<slots_t>(slot));
				if (item && item->hasImbuements()) {
					found = true;
				}
			}
			hasImbuedItemEquipped = found;
		}

		void setChaseMode(bool mode);
	
		void setFightMode(fightMode_t mode) {
			fightMode = mode;
		}
	
		void setSecureMode(bool mode) {
			secureMode = mode;
		}

		//combat functions
		bool setAttackedCreature(const CreaturePtr& creature) override;
		bool isImmune(CombatType_t type) const override;
		bool isImmune(ConditionType_t type) const override;
		bool hasShield() const;
		bool isAttackable() const override;
		static bool lastHitIsPlayer(const CreaturePtr& lastHitCreature);

		void changeHealth(int32_t healthChange, bool sendHealthChange = true) override;
		void changeMana(int32_t manaChange);
		void changeSoul(int32_t soulChange);
		void addSoul(uint8_t soulChange);
		void addStamina(uint16_t gain);
		void changeStamina(int32_t amount);

		bool isPzLocked() const {
			return pzLocked;
		}
	
		BlockType_t blockHit(const CreaturePtr& attacker, CombatType_t combatType, int32_t& damage,
		                             bool checkDefense = false, bool checkArmor = false, bool field = false, bool ignoreResistances = false) override;
	
		void doAttacking(uint32_t interval) override;
	
		bool hasExtraSwing() override {
			return lastAttack > 0 && ((OTSYS_TIME() - lastAttack) >= getAttackSpeed());
		}

		uint16_t getSpecialSkill(uint8_t skill) const {
			return std::max<int32_t>(0, varSpecialSkills[skill]);
		}
	
		uint16_t getSkillLevel(uint8_t skill) const {
			return std::max<int32_t>(0, skills[skill].level + varSkills[skill]);
		}
	
		uint16_t getBaseSkill(uint8_t skill) const {
			return skills[skill].level;
		}
	
		uint8_t getSkillPercent(uint8_t skill) const {
			return skills[skill].percent;
		}

		bool getAddAttackSkill() const {
			return addAttackSkillPoint;
		}
	
		BlockType_t getLastAttackBlockType() const {
			return lastAttackBlockType;
		}

		ItemPtr getWeapon(slots_t slot, bool ignoreAmmo) const;
		ItemPtr getWeapon(bool ignoreAmmo = false) const;
		WeaponType_t getWeaponType() const;
		int32_t getWeaponSkill(const ItemConstPtr& item) const;

		void drainHealth(const CreaturePtr& attacker, int32_t damage) override;
		void drainMana(const CreaturePtr& attacker, int32_t manaLoss);
		void addManaSpent(uint64_t amount);
		void removeManaSpent(uint64_t amount, bool notify = false);
		void addSkillAdvance(skills_t skill, uint64_t count);
		void removeSkillTries(skills_t skill, uint64_t count, bool notify = false);

		int32_t getArmor() const override;
		int32_t getDefense() const override;
		float getAttackFactor() const override;
		float getDefenseFactor() const override;

		void addInFightTicks(bool pzlock = false);

		uint64_t getGainedExperience(const CreaturePtr& attacker) const override;

		//combat event functions
		void onAddCondition(ConditionType_t type) override;
		void onAddCombatCondition(ConditionType_t type) override;
		void onEndCondition(ConditionType_t type) override;
		void onCombatRemoveCondition(Condition* condition) override;
		void onAttackedCreature(const CreaturePtr& target, bool addFightTicks = true) override;
		void onAttacked() override;
		void onAttackedCreatureDrainHealth(const CreaturePtr& target, int32_t points) override;
		void onTargetCreatureGainHealth(const CreaturePtr& target, int32_t points) override;
		bool onKilledCreature(const CreaturePtr& target, bool lastHit = true) override;
		void onGainExperience(uint64_t gainExp, const CreaturePtr& target) override;
		void onGainSharedExperience(uint64_t gainExp, const CreaturePtr& source);
		void onAttackedCreatureBlockHit(BlockType_t blockType) override;
		void onBlockHit() override;
		void onChangeZone(ZoneType_t zone) override;
		void onAttackedCreatureChangeZone(ZoneType_t zone) override;
		void onIdleStatus() override;
		void onPlacedCreature() override;

		LightInfo getCreatureLight() const override;

		Skulls_t getSkull() const override;
		Skulls_t getSkullClient(const CreatureConstPtr& creature) const override;
		int64_t getSkullTicks() const { return skullTicks; }
		void setSkullTicks(int64_t ticks) { skullTicks = ticks; }

		bool hasAttacked(const PlayerConstPtr& attacked) const;
		void addAttacked(const PlayerConstPtr& attacked);
		void removeAttacked(const PlayerConstPtr& attacked);
		void clearAttacked();
		void addUnjustifiedDead(const PlayerConstPtr& attacked);
	
		void sendCreatureSkull(const CreatureConstPtr& creature) const {
			if (client) {
				client->sendCreatureSkull(creature);
			}
		}
	
		void checkSkullTicks(int64_t ticks);

		bool canWear(uint32_t lookType, uint8_t addons) const;
		bool hasOutfit(uint32_t lookType, uint8_t addons) const;
		void addOutfit(uint16_t lookType, uint8_t addons);
		bool removeOutfit(uint16_t lookType);
		bool removeOutfitAddon(uint16_t lookType, uint8_t addons);
		bool getOutfitAddons(const Outfit& outfit, uint8_t& addons) const;

		size_t getMaxVIPEntries() const;
		size_t getMaxDepotItems() const;

		//tile
		//send methods
		void sendAddTileItem(const TileConstPtr& tile, const Position& pos, const ItemConstPtr& item) const {
			if (client) {
				int32_t stackpos = tile->getStackposOfItem(this->getPlayer(), item);
				if (stackpos != -1) {
					client->sendAddTileItem(pos, stackpos, item);
				}
			}
		}
	
		void sendUpdateTileItem(const TileConstPtr& tile, const Position& pos, const ItemConstPtr& item) const {
			if (client) {
				int32_t stackpos = tile->getStackposOfItem(this->getPlayer(), item);
				if (stackpos != -1) {
					client->sendUpdateTileItem(pos, stackpos, item);
				}
			}
		}
	
		void sendRemoveTileThing(const Position& pos, int32_t stackpos) const {
			if (stackpos != -1 && client) {
				client->sendRemoveTileThing(pos, stackpos);
			}
		}
	
		void sendUpdateTileCreature(const CreatureConstPtr& creature) const {
			if (client) {
				client->sendUpdateTileCreature(creature->getPosition(), creature->getTile()->getClientIndexOfCreature(this->getPlayer(), creature), creature);
			}
		}
	
		void sendRemoveTileCreature(const CreatureConstPtr& creature, const Position& pos, int32_t stackpos) const {
			if (client) {
				client->sendRemoveTileCreature(creature, pos, stackpos);
			}
		}
	
		void sendUpdateTile(const TileConstPtr& tile, const Position& pos) const {
			if (client) {
				client->sendUpdateTile(tile, pos);
			}
		}

		void sendChannelMessage(const std::string& author, const std::string& text, SpeakClasses type, uint16_t channel) const {
			if (client) {
				client->sendChannelMessage(author, text, type, channel);
			}
		}
	
		void sendChannelEvent(uint16_t channelId, const std::string& playerName, ChannelEvent_t channelEvent) const {
			if (client) {
				client->sendChannelEvent(channelId, playerName, channelEvent);
			}
		}
	
		void sendCreatureAppear(const CreatureConstPtr& creature, const Position& pos, MagicEffectClasses magicEffect = CONST_ME_NONE) {
			if (client) {
				client->sendAddCreature(creature, pos, creature->getTile()->getClientIndexOfCreature(this->getPlayer(), creature), magicEffect);
			}
		}
	
		void sendCreatureMove(const CreatureConstPtr& creature, const Position& newPos, int32_t newStackPos, const Position& oldPos, int32_t oldStackPos, bool teleport) const {
			if (client) {
				client->sendMoveCreature(creature, newPos, newStackPos, oldPos, oldStackPos, teleport);
			}
		}
	
		void sendCreatureTurn(const CreatureConstPtr& creature) {
			if (client && canSeeCreature(creature)) {
				int32_t stackpos = creature->getTile()->getClientIndexOfCreature(this->getPlayer(), creature);
				if (stackpos != -1) {
					client->sendCreatureTurn(creature, stackpos);
				}
			}
		}
	
		void sendCreatureSay(const CreatureConstPtr& creature, SpeakClasses type, const std::string& text, const Position* pos = nullptr) const {
			if (client) {
				client->sendCreatureSay(creature, type, text, pos);
			}
		}
	
		void sendPrivateMessage(const PlayerConstPtr& speaker, SpeakClasses type, const std::string& text) const {
			if (client) {
				client->sendPrivateMessage(speaker, type, text);
			}
		}
	
		void sendCreatureSquare(const CreatureConstPtr& creature, SquareColor_t color) const {
			if (client) {
				client->sendCreatureSquare(creature, color);
			}
		}
	
		void sendCreatureChangeOutfit(const CreatureConstPtr& creature, const Outfit_t& outfit) const {
			if (client) {
				client->sendCreatureOutfit(creature, outfit);
			}
		}
	
		void sendCreatureChangeVisible(const CreatureConstPtr& creature, bool visible) const {
			if (!client) {
				return;
			}

			if (creature->getPlayer()) {
				if (visible) {
					client->sendCreatureOutfit(creature, creature->getCurrentOutfit());
				} else {
					static Outfit_t outfit;
					client->sendCreatureOutfit(creature, outfit);
				}
			} else if (canSeeInvisibility()) {
				client->sendCreatureOutfit(creature, creature->getCurrentOutfit());
			} else {
				int32_t stackpos = creature->getTile()->getClientIndexOfCreature(this->getPlayer(), creature);
				if (stackpos == -1) {
					return;
				}

				if (visible) {
					client->sendAddCreature(creature, creature->getPosition(), stackpos);
				} else {
					client->sendRemoveTileCreature(creature, creature->getPosition(), stackpos);
				}
			}
		}
	
		void sendCreatureLight(const CreatureConstPtr& creature) const {
			if (client) {
				client->sendCreatureLight(creature);
			}
		}
	
		void sendCreatureWalkthrough(const CreatureConstPtr& creature, bool walkthrough) const {
			if (client) {
				client->sendCreatureWalkthrough(creature, walkthrough);
			}
		}
	
		void sendCreatureShield(const CreatureConstPtr& creature) const {
			if (client) {
				client->sendCreatureShield(creature);
			}
		}
	
		void sendCreatureType(uint32_t creatureId, uint8_t creatureType) const {
			if (client) {
				client->sendCreatureType(creatureId, creatureType);
			}
		}
	
		void sendCreatureHelpers(uint32_t creatureId, uint16_t helpers) const {
			if (client) {
				client->sendCreatureHelpers(creatureId, helpers);
			}
		}
	
		void sendSpellCooldown(uint8_t spellId, uint32_t time) const {
			if (client) {
				client->sendSpellCooldown(spellId, time);
			}
		}
	
		void sendSpellGroupCooldown(SpellGroup_t groupId, uint32_t time) const {
			if (client) {
				client->sendSpellGroupCooldown(groupId, time);
			}
		}
	
		void sendModalWindow(const ModalWindow& modalWindow);

		//container
		void sendAddContainerItem(const ContainerConstPtr& container, ItemPtr& item) const;
		void sendUpdateContainerItem(const ContainerConstPtr& container, uint16_t slot, const ItemConstPtr& newItem) const;
		void sendRemoveContainerItem(const ContainerConstPtr& container, uint16_t slot);
	
		void sendContainer(uint8_t cid, const ContainerConstPtr& container, bool hasParent, uint16_t firstIndex) const {
			if (client) {
				client->sendContainer(cid, container, hasParent, firstIndex);
			}
		}

		//inventory
		void sendInventoryItem(slots_t slot, const ItemConstPtr& item) const {
			if (client) {
				client->sendInventoryItem(slot, item);
			}
		}
	
		void sendItems() const {
			if (client) {
				client->sendItems();
			}
		}

		void sendQuiverUpdate(bool sendAll = false)	{
			if (!sendAll) {
				// update one slot
				auto slotThing = getThing(CONST_SLOT_RIGHT);
				if (slotThing) {
					auto slotItem = slotThing->getItem();
					if (slotItem && slotItem->getWeaponType() == WEAPON_QUIVER) {
						sendInventoryItem(CONST_SLOT_RIGHT, slotItem);
					}
				}
			} else {
				// update all slots
				std::vector<slots_t> slots = {CONST_SLOT_RIGHT, CONST_SLOT_LEFT, CONST_SLOT_AMMO};
				for (auto const& slot : slots) {
					auto slotThing = getThing(slot);
					if (slotThing) {
						auto slotItem = slotThing->getItem();
						if (slotItem && slotItem->getWeaponType() == WEAPON_QUIVER) {
							sendInventoryItem(slot, slotItem);
						}
					}
				}
			}
		}

		//event methods
		void onUpdateTileItem(const TilePtr& tile, const Position& pos, const ItemPtr& oldItem,
		                              const ItemType& oldType, const ItemPtr& newItem, const ItemType& newType) override;
		void onRemoveTileItem(const TilePtr& tile, const Position& pos, const ItemType& iType,
		                              const ItemPtr& item) override;

		void onCreatureAppear(const CreaturePtr& creature, bool isLogin) override;
		void onRemoveCreature(const CreaturePtr& creature, bool isLogout) override;
		void onCreatureMove(const CreaturePtr& creature, const TilePtr& newTile, const Position& newPos,
		                            const TilePtr& oldTile, const Position& oldPos, bool teleport) override;

		void onAttackedCreatureDisappear(bool isLogout) override;
		void onFollowCreatureDisappear(bool isLogout) override;

		//container
		void onAddContainerItem(ItemPtr item);
		void onUpdateContainerItem(ContainerPtr container, ItemPtr oldItem, ItemPtr newItem);
		void onRemoveContainerItem(ContainerPtr container, ItemPtr item);

		void onCloseContainer(ContainerPtr container);
		void onSendContainer(ContainerPtr container);
		void autoCloseContainers(ContainerPtr container);

		//inventory
		void onUpdateInventoryItem(ItemPtr oldItem, ItemPtr newItem);
		void onRemoveInventoryItem(ItemPtr item);

		void sendCancelMessage(const std::string& msg) const {
			if (client) {
				client->sendTextMessage(TextMessage(MESSAGE_STATUS_SMALL, msg));
			}
		}
	
		void sendCancelMessage(ReturnValue message) const;
	
		void sendCancelTarget() const {
			if (client) {
				client->sendCancelTarget();
			}
		}
	
		void sendCancelWalk() const {
			if (client) {
				client->sendCancelWalk();
			}
		}
	
		void sendChangeSpeed(const CreatureConstPtr& creature, uint32_t newSpeed) const {
			if (client) {
				client->sendChangeSpeed(creature, newSpeed);
			}
		}
	
		void sendCreatureHealth(const CreatureConstPtr& creature) const {
			if (client) {
				client->sendCreatureHealth(creature);
			}
		}
	
		void sendDistanceShoot(const Position& from, const Position& to, unsigned char type) const {
			if (client) {
				client->sendDistanceShoot(from, to, type);
			}
		}
		void sendHouseWindow(House* house, uint32_t listId) const;

		void sendAccountManagerTextWindow(uint32_t id, const std::string& text) const {
			if (client) {
				client->sendAccountManagerTextBox(id, text);
			}
		}
	
		void sendCreatePrivateChannel(uint16_t channelId, const std::string& channelName) const {
			if (client) {
				client->sendCreatePrivateChannel(channelId, channelName);
			}
		}
	
		void sendClosePrivate(uint16_t channelId);
	
		void sendIcons() const {
			if (client) {
				client->sendIcons(getClientIcons());
			}
		}
	
		void sendMagicEffect(const Position& pos, uint8_t type) const {
			if (client) {
				client->sendMagicEffect(pos, type);
			}
		}
	
		void sendPing();
	
		void sendPingBack() const {
			if (client) {
				client->sendPingBack();
			}
		}
	
		void sendStats();
	
		void sendBasicData() const {
			if (client) {
				client->sendBasicData();
			}
		}
	
		void sendSkills() const {
			if (client) {
				client->sendSkills();
			}
		}
	
		void sendTextMessage(MessageClasses mclass, const std::string& message) const {
			if (client) {
				client->sendTextMessage(TextMessage(mclass, message));
			}
		}
	
		void sendTextMessage(const TextMessage& message) const {
			if (client) {
				client->sendTextMessage(message);
			}
		}
	
		void sendReLoginWindow(uint8_t unfairFightReduction) const {
			if (client) {
				client->sendReLoginWindow(unfairFightReduction);
			}
		}
	
		void sendTextWindow(const ItemPtr& item, uint16_t maxlen, bool canWrite) const {
			if (client) {
				client->sendTextWindow(windowTextId, item, maxlen, canWrite);
			}
		}
	
		void sendTextWindow(uint32_t itemId, const std::string& text) const {
			if (client) {
				client->sendTextWindow(windowTextId, itemId, text);
			}
		}
	
		void sendToChannel(const CreatureConstPtr& creature, SpeakClasses type, const std::string& text, uint16_t channelId) const {
			if (client) {
				client->sendToChannel(creature, type, text, channelId);
			}
		}
	
		void sendShop(const NpcPtr& npc) const {
			if (client) {
				client->sendShop(npc, shopItemList);
			}
		}
	
		void sendSaleItemList() const {
			if (client) {
				client->sendSaleItemList(shopItemList);
			}
		}
	
		void sendCloseShop() const {
			if (client) {
				client->sendCloseShop();
			}
		}
	
		void sendMarketEnter() const {
			if (client) {
				client->sendMarketEnter();
			}
		}
	
		void sendMarketLeave() {
			inMarket = false;
			if (client) {
				client->sendMarketLeave();
			}
		}
	
		void sendMarketBrowseItem(uint16_t itemId, const MarketOfferList& buyOffers, const MarketOfferList& sellOffers) const {
			if (client) {
				client->sendMarketBrowseItem(itemId, buyOffers, sellOffers);
			}
		}
	
		void sendMarketBrowseOwnOffers(const MarketOfferList& buyOffers, const MarketOfferList& sellOffers) const {
			if (client) {
				client->sendMarketBrowseOwnOffers(buyOffers, sellOffers);
			}
		}
	
		void sendMarketBrowseOwnHistory(const HistoryMarketOfferList& buyOffers, const HistoryMarketOfferList& sellOffers) const {
			if (client) {
				client->sendMarketBrowseOwnHistory(buyOffers, sellOffers);
			}
		}
	
		void sendMarketDetail(uint16_t itemId) const {
			if (client) {
				client->sendMarketDetail(itemId);
			}
		}
	
		void sendMarketAcceptOffer(const MarketOfferEx& offer) const {
			if (client) {
				client->sendMarketAcceptOffer(offer);
			}
		}
	
		void sendMarketCancelOffer(const MarketOfferEx& offer) const {
			if (client) {
				client->sendMarketCancelOffer(offer);
			}
		}
	
		void sendTradeItemRequest(const std::string& traderName, const ItemPtr& item, bool ack) const {
			if (client) {
				client->sendTradeItemRequest(traderName, item, ack);
			}
		}
	
		void sendTradeClose() const {
			if (client) {
				client->sendCloseTrade();
			}
		}
	
		void sendWorldLight(LightInfo lightInfo) const {
			if (client) {
				client->sendWorldLight(lightInfo);
			}
		}
	
		void sendChannelsDialog() const {
			if (client) {
				client->sendChannelsDialog();
			}
		}
	
		void sendOpenPrivateChannel(const std::string& receiver) const {
			if (client) {
				client->sendOpenPrivateChannel(receiver);
			}
		}
	
		void sendOutfitWindow() const {
			if (client) {
				client->sendOutfitWindow();
			}
		}
	
		void sendCloseContainer(uint8_t cid) const {
			if (client) {
				client->sendCloseContainer(cid);
			}
		}

		void sendChannel(uint16_t channelId, const std::string& channelName, const UsersMap* channelUsers, const InvitedMap* invitedUsers) const {
			if (client) {
				client->sendChannel(channelId, channelName, channelUsers, invitedUsers);
			}
		}
	
		void sendTutorial(uint8_t tutorialId) const {
			if (client) {
				client->sendTutorial(tutorialId);
			}
		}
	
		void sendAddMarker(const Position& pos, uint8_t markType, const std::string& desc) const {
			if (client) {
				client->sendAddMarker(pos, markType, desc);
			}
		}
		
		void sendQuestLog() const {
			if (client) {
				client->sendQuestLog();
			}
		}
		
		void sendQuestLine(const Quest* quest) const {
			if (client) {
				client->sendQuestLine(quest);
			}
		}
	
		void sendEnterWorld() const {
			if (client) {
				client->sendEnterWorld();
			}
		}
	
		void sendFightModes() const {
			if (client) {
				client->sendFightModes();
			}
		}
	
		void sendNetworkMessage(const NetworkMessage& message) const {
			if (client) {
				client->writeToOutputBuffer(message);
			}
		}

		void receivePing() {
			lastPong = OTSYS_TIME();
		}

		void onThink(uint32_t interval) override;

		void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

		void setNextAction(int64_t time) {
			if (time > nextAction) {
				nextAction = time;
			}
		}
	
		bool canDoAction() const {
			return nextAction <= OTSYS_TIME();
		}
	
		uint32_t getNextActionTime() const;

		ItemPtr getWriteItem(uint32_t& windowTextId, uint16_t& maxWriteLen);
		void setWriteItem(const ItemPtr& item, uint16_t maxWriteLen = 0);

		House* getEditHouse(uint32_t& windowTextId, uint32_t& listId) const;
		void setEditHouse(House* house, uint32_t listId = 0);

		void learnInstantSpell(const std::string& spellName);
		void forgetInstantSpell(const std::string& spellName);
		bool hasLearnedInstantSpell(const std::string& spellName) const;

		void updateRegeneration() const;

		void addItemImbuements(const ItemPtr& item);
		void removeItemImbuements(const ItemPtr& item);
		void addImbuementEffect(const std::shared_ptr<Imbuement>& imbue);
		void removeImbuementEffect(const std::shared_ptr<Imbuement>& imbue);

		CreatureType_t getCreatureType(const MonsterPtr& monster) const;

		// To-do : Make all these methods into const
		gtl::node_hash_map<uint8_t, std::vector<std::shared_ptr<DamageModifier>>> getAttackModifiers() const;
		gtl::node_hash_map<uint8_t, std::vector<std::shared_ptr<DamageModifier>>> getDefenseModifiers() const;

		gtl::node_hash_map<uint8_t, ModifierTotals> getConvertedTotals(const uint8_t modType, const CombatType_t damageType, const CombatOrigin originType, const CreatureType_t creatureType, const RaceType_t race, const std::string_view creatureName);

		gtl::node_hash_map<uint8_t, ModifierTotals> getAttackModifierTotals(const CombatType_t damageType, const CombatOrigin originType, const CreatureType_t creatureType, const RaceType_t race, const std::string_view creatureName) const;
		gtl::node_hash_map<uint8_t, ModifierTotals> getDefenseModifierTotals(const CombatType_t damageType, const CombatOrigin originType, const CreatureType_t creatureType, const RaceType_t race, const std::string_view creatureName) const;

		std::vector<Position> getOpenPositionsInRadius(int radius) const;

		const bool addAugment(std::string_view augmentName);
		const bool addAugment(const std::shared_ptr<Augment>& augment);

		const bool removeAugment(std::string_view augmentName);
		const bool removeAugment(const std::shared_ptr<Augment>& augment);

		const bool isAugmented() const;
		const bool hasAugment(const std::string_view augmentName, const bool checkItems);
		const bool hasAugment(const std::shared_ptr<Augment>& augmentName, const bool checkItems);
		const std::vector<std::shared_ptr<Augment>>& getPlayerAugments() const;

		// To-do : convert all these params to const and ref.
		void absorbDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, int32_t percent, int32_t flat);
		void restoreManaFromDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, int32_t percent, int32_t flat);
		void reviveSoulFromDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, int32_t percent, int32_t flat);
		void replenishStaminaFromDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, int32_t percent, int32_t flat);
		void resistDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, int32_t percent, int32_t flat) const;
		void reflectDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, int32_t percent, int32_t flat, uint8_t areaEffect, uint8_t distanceEffect);
		void deflectDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, int32_t percent, int32_t flat, CombatOrigin paramOrigin, uint8_t areaEffect, uint8_t distanceEffect);
		void ricochetDamage(CombatDamage& originalDamage, int32_t percent, int32_t flat, uint8_t areaEffect, uint8_t distanceEffect);
		void convertDamage(const CreaturePtr& target, CombatDamage& originalDamage, gtl::node_hash_map<uint8_t, ModifierTotals> conversionList);
		void reformDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, gtl::node_hash_map<uint8_t, ModifierTotals> conversionList);
		void increaseDamage(std::optional<CreaturePtr> attackerOpt, CombatDamage& originalDamage, int32_t percent, int32_t flat) const;

		uint8_t getAccountManagerLastState() 
		{
			return accountManagerState;
		}

		void setAccountManagerLastState(uint8_t state) 
		{
			accountManagerState = state;
		}

		void setTempAccountName(std::string name) 
		{
			client->setTempAccountName(name);
		}

		void setTempPassword(std::string password) 
		{
			client->setTempPassword(password);
		}

		void setTempPosition(Position spawn_pos) 
		{
			client->setTempPosition(spawn_pos);
		}

		void setTempTownId(uint32_t id)
		{
			client->setTempTownId(id);
		}

		void setTempCharacterChoice(uint32_t choice)
		{
			client->setTempCharacterChoice(choice);
		}

		void setTempVocation(uint32_t vocation)
		{
			client->setTempVocation(vocation);
		}

		const std::string getTempAccountName() 
		{
			return client->getTempAccountName();
		}

		const std::string getTempPassword() 
		{
			return client->getTempPassword();
		}

		const Position getTempPosition() 
		{
			return client->getTempPosition();
		}

		const uint32_t getTempVocation()
		{
			return client->getTempVocation();
		}

		const uint32_t getTempCharacterChoice()
		{
			return client->getTempCharacterChoice();
		}

		const uint32_t getTempTownId()
		{
			return client->getTempTownId();
		}

		std::vector<ItemPtr> getEquipment(bool validateSlot = true) const;

		Position generateAttackPosition(std::optional<CreaturePtr> attacker, Position& defensePosition, CombatOrigin origin);

		std::unique_ptr<AreaCombat> generateDeflectArea(std::optional<CreaturePtr> attacker, int32_t targetCount) const;

	private:
		std::forward_list<Condition*> getMuteConditions() const;

		void checkTradeState(const ItemPtr& item);
		bool hasCapacity(const ItemPtr& item, uint32_t count) const;

		void gainExperience(uint64_t gainExp, const CreaturePtr& source);
		void addExperience(const CreaturePtr& source, uint64_t exp, bool sendText = false);
		void removeExperience(uint64_t exp, bool sendText = false);

		void updateInventoryWeight();

		void setNextWalkActionTask(SchedulerTask* task);
		void setNextActionTask(SchedulerTask* task, bool resetIdleTime = true);

		void death(const CreaturePtr& lastHitCreature) override;
		bool dropCorpse(const CreaturePtr& lastHitCreature, const CreaturePtr& mostDamageCreature, bool lastHitUnjustified, bool mostDamageUnjustified) override;
		ItemPtr getCorpse(const CreaturePtr& lastHitCreature, const CreaturePtr& mostDamageCreature) override;

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                     uint32_t flags, CreaturePtr actor = nullptr) override;
		ReturnValue queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t& maxQueryCount,
				uint32_t flags) override;
		ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr) override;
		CylinderPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem,
				uint32_t& flags) override; // another optional ref wrapper

		void addThing(ThingPtr) override {}
		void addThing(int32_t index, ThingPtr thing) override;

		void updateThing(ThingPtr thing, uint16_t itemId, uint32_t count) override;
		void replaceThing(uint32_t index, ThingPtr thing) override;

		void removeThing(ThingPtr thing, uint32_t count) override;

		int32_t getThingIndex(ThingPtr thing) override;
		size_t getFirstIndex() const override;
		size_t getLastIndex() const override;
		uint32_t getItemTypeCount(uint16_t itemId, int32_t subType = -1) const override;
		gtl::btree_map<uint32_t, uint32_t>& getAllItemTypeCount(gtl::btree_map<uint32_t, uint32_t>& countMap) const override;
		ThingPtr getThing(size_t index) override;

		void internalAddThing(ThingPtr thing) override;
		void internalAddThing(uint32_t index, ThingPtr thing) override;

		std::unordered_set<uint32_t> attackedSet;
		std::unordered_set<uint32_t> VIPList;

		std::map<uint8_t, OpenContainer> openContainers;
		std::map<uint32_t, DepotChestPtr> depotChests;
		gtl::btree_map<uint32_t, int32_t> storageMap;

		std::vector<std::shared_ptr<Augment>> augments;

		std::vector<OutfitEntry> outfits;

		std::list<ShopInfo> shopItemList;

		// todo cleanup all these wasteful containers below and above, to reduce the insane size of player objects.
		std::forward_list<PartyPtr> invitePartyList;
		std::forward_list<uint32_t> modalWindows;
		std::forward_list<std::string> learnedInstantSpellList;
		std::forward_list<Condition*> storedConditionList; // TODO: This variable is only temporarily used when logging in, get rid of it somehow

		std::string name;
		std::string guildNick;
		std::string tempAccountName;
		std::string tempPassword;

		Skill skills[SKILL_LAST + 1];
		LightInfo itemsLight;
		Position loginPosition;
		Position lastWalkthroughPosition;

		time_t lastLoginSaved = 0;
		time_t lastLogout = 0;
		time_t premiumEndsAt = 0;

		uint64_t experience = 0;
		uint64_t manaSpent = 0;
		uint64_t lastAttack = 0;
		uint64_t bankBalance = 0;
		uint64_t lastQuestlogUpdate = 0;
		int64_t lastFailedFollow = 0;
		int64_t skullTicks = 0;
		int64_t lastWalkthroughAttempt = 0;
		int64_t lastToggleMount = 0;
		int64_t lastPing;
		int64_t lastPong;
		int64_t nextAction = 0;
		ProtocolGame_ptr client;

		BedItemPtr bedItem = nullptr;
		Guild_ptr guild = nullptr;
		GuildRank_ptr guildRank = nullptr;
		Group* group = nullptr;
		InboxPtr inbox;
		ItemPtr tradeItem = nullptr;
 		ItemPtr inventory[CONST_SLOT_LAST + 1] = {};
		ItemPtr writeItem = nullptr;
		House* editHouse = nullptr;
		NpcPtr shopOwner = nullptr;
		PlayerPtr tradePartner = nullptr;
		SchedulerTask* walkTask = nullptr;
		Town* town = nullptr;
		Vocation* vocation = nullptr;
		StoreInboxPtr storeInbox = nullptr;
		RewardChestPtr rewardChest = nullptr;
		DepotLockerPtr depotLocker = nullptr;

		uint32_t party = 0;
		uint32_t inventoryWeight = 0;
		uint32_t capacity = 40000;
		uint32_t damageImmunities = 0;
		uint32_t conditionImmunities = 0;
		uint32_t conditionSuppressions = 0;
		uint32_t level = 1;
		uint32_t magLevel = 0;
		uint32_t actionTaskEvent = 0;
		uint32_t walkTaskEvent = 0;
		uint32_t classicAttackEvent = 0;
		uint32_t MessageBufferTicks = 0;
		uint32_t lastIP = 0;
		uint32_t accountNumber = 0;
		uint32_t guid = 0;
		uint32_t windowTextId = 0;
		uint32_t editListId = 0;
		uint32_t mana = 0;
		uint32_t manaMax = 0;
		int32_t varSkills[SKILL_LAST + 1] = {};
		int32_t varSpecialSkills[SPECIALSKILL_LAST + 1] = {};
		int32_t varStats[STAT_LAST + 1] = {};
		int32_t purchaseCallback = -1;
		int32_t saleCallback = -1;
		int32_t MessageBufferCount = 0;
		int32_t bloodHitCount = 0;
		int32_t shieldBlockCount = 0;
		int32_t offlineTrainingSkill = -1;
		int32_t offlineTrainingTime = 0;
		int32_t idleTime = 0;

		uint16_t lastStatsTrainingTime = 0;
		uint16_t staminaMinutes = 2520;
		uint16_t maxWriteLen = 0;

		uint8_t soul = 0;
		std::bitset<6> blessings;
		uint8_t levelPercent = 0;
		uint8_t magLevelPercent = 0;
		uint8_t accountManagerState = 0;


		PlayerSex_t sex = PLAYERSEX_FEMALE;
		OperatingSystem_t operatingSystem = CLIENTOS_NONE;
		BlockType_t lastAttackBlockType = BLOCK_NONE;
		tradestate_t tradeState = TRADE_NONE;
		fightMode_t fightMode = FIGHTMODE_ATTACK;
		AccountType_t accountType = ACCOUNT_TYPE_NORMAL;

		bool chaseMode = false;
		bool secureMode = false;
		bool inMarket = false;
		bool wasMounted = false;
		bool ghostMode = false;
		bool pzLocked = false;
		bool isConnecting = false;
		bool addAttackSkillPoint = false;
		bool hasImbuedItemEquipped = false;
		bool inventoryAbilities[CONST_SLOT_LAST + 1] = {};

		static uint32_t playerAutoID;

		void updateItemsLight(bool internal = false);
		int32_t getStepSpeed() const override {
			return std::max<int32_t>(PLAYER_MIN_SPEED, std::min<int32_t>(PLAYER_MAX_SPEED, getSpeed()));
		}
		void updateBaseSpeed() {
			if (!hasFlag(PlayerFlag_SetMaxSpeed)) {
				baseSpeed = vocation->getBaseSpeed() + (2 * (level - 1));
			} else {
				baseSpeed = PLAYER_MAX_SPEED;
			}
		}

		bool isPromoted() const;

		uint32_t getAttackSpeed() const;

		static uint8_t getPercentLevel(uint64_t count, uint64_t nextLevelCount);
		double getLostPercent() const;
	
		uint64_t getLostExperience() const override {
			return skillLoss ? static_cast<uint64_t>(experience * getLostPercent()) : 0;
		}
	
		uint32_t getDamageImmunities() const override {
			return damageImmunities;
		}
	
		uint32_t getConditionImmunities() const override {
			return conditionImmunities;
		}
	
		uint32_t getConditionSuppressions() const override {
			return conditionSuppressions;
		}
	
		uint16_t getLookCorpse() const override;
		void getPathSearchParams(const CreatureConstPtr& creature, FindPathParams& fpp) const override;

		friend class Game;
		friend class Npc;
		friend class LuaScriptInterface;
		friend class Map;
		friend class Actions;
		friend class IOLoginData;
		friend class ProtocolGame;
};

#endif
