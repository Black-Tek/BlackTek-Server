// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_CREATURE_H
#define FS_CREATURE_H

#include "map.h"
#include "position.h"
#include "condition.h"
#include "const.h"
#include "tile.h"
#include "enums.h"
#include "creatureevent.h"
#include "declarations.h"
#include "skills.h"

class Map;
using ConditionList = std::list<Condition*>;
using CreatureEventList = std::list<CreatureEvent*>;
using namespace Components::Skills;

enum slots_t : uint8_t {
	CONST_SLOT_WHEREEVER = 0,
	CONST_SLOT_HEAD = 1,
	CONST_SLOT_NECKLACE = 2,
	CONST_SLOT_BACKPACK = 3,
	CONST_SLOT_ARMOR = 4,
	CONST_SLOT_RIGHT = 5,
	CONST_SLOT_LEFT = 6,
	CONST_SLOT_LEGS = 7,
	CONST_SLOT_FEET = 8,
	CONST_SLOT_RING = 9,
	CONST_SLOT_AMMO = 10,
	CONST_SLOT_STORE_INBOX = 11,

	CONST_SLOT_FIRST = CONST_SLOT_HEAD,
	CONST_SLOT_LAST = CONST_SLOT_AMMO,
};

struct FindPathParams {
	bool fullPathSearch = true;
	bool clearSight = true;
	bool allowDiagonal = true;
	bool keepDistance = false;
	int32_t maxSearchDist = 0;
	int32_t minTargetDist = -1;
	int32_t maxTargetDist = -1;
};
static constexpr int32_t EVENT_DUMP_DECAY = 250;
static constexpr int32_t EVENT_CREATURECOUNT = 10;
static constexpr int32_t EVENT_CREATURE_THINK_INTERVAL = 1000;
static constexpr int32_t EVENT_CHECK_CREATURE_INTERVAL = (EVENT_CREATURE_THINK_INTERVAL / EVENT_CREATURECOUNT);

class FrozenPathingConditionCall
{
	public:
		explicit FrozenPathingConditionCall(Position targetPos) : targetPos(std::move(targetPos)) {}

		bool operator()(const Position& startPos, const Position& testPos,
		                const FindPathParams& fpp, int32_t& bestMatchDist) const;

		bool isInRange(const Position& startPos, const Position& testPos,
		               const FindPathParams& fpp) const;

	private:
		Position targetPos;
};

//////////////////////////////////////////////////////////////////////
// Defines the Base class for all creatures and base functions which
// every creature has

class Creature : virtual public Thing, public SharedObject
{
	protected:
		Creature();

	public:
		static double speedA, speedB, speedC;

		virtual ~Creature();

		// non-copyable
		Creature(const Creature&) = delete;
		Creature& operator=(const Creature&) = delete;

		CreaturePtr getCreature() override final {
			return static_shared_this<Creature>();
		}
	
		CreatureConstPtr getCreature() const override final {
			return static_shared_this<Creature>();
		}
	
		virtual PlayerPtr getPlayer() {
			return nullptr;
		}
	
		virtual PlayerConstPtr getPlayer() const {
			return nullptr;
		}
	
		virtual NpcPtr getNpc() {
			return nullptr;
		}
	
		virtual NpcConstPtr getNpc() const {
			return nullptr;
		}
	
		virtual MonsterPtr getMonster() {
			return nullptr;
		}
	
		virtual MonsterConstPtr getMonster() const {
			return nullptr;
		}

		virtual const std::string& getRegisteredName() const = 0;

		virtual const std::string& getName() const = 0;
		virtual const std::string& getNameDescription() const = 0;
	
		virtual CreatureType_t getType(CreaturePtr caller = nullptr) const = 0;
		virtual void setID() = 0;
	
		void setRemoved() {
			isInternalRemoved = true;
		}

		uint32_t getID() const {
			return id;
		}
	
		virtual void removeList() = 0;
		virtual void addList() = 0;

		virtual bool canSee(const Position& pos) const;
		virtual bool canSeeCreature(const CreatureConstPtr& creature) const;

		virtual RaceType_t getRace() const {
			return RACE_NONE;
		}
	
		virtual Skulls_t getSkull() const {
			return skull;
		}
	
		virtual Skulls_t getSkullClient(const CreatureConstPtr& creature) const {
			return creature->getSkull();
		}
	
		void setSkull(Skulls_t newSkull);
	
		Direction getDirection() const {
			return direction;
		}
	
		void setDirection(Direction dir) {
			direction = dir;
		}

		bool isHealthHidden() const {
			return hiddenHealth;
		}
	
		void setHiddenHealth(bool b) {
			hiddenHealth = b;
		}

		int32_t getThrowRange() const override final {
			return 1;
		}
	
		bool isPushable() const override {
			return getWalkDelay() <= 0;
		}
	
		bool isRemoved() const override final {
			return isInternalRemoved;
		}
	
		virtual bool canSeeInvisibility() const {
			return false;
		}
	
		virtual bool isInGhostMode() const {
			return false;
		}
	
		virtual bool canSeeGhostMode(const CreatureConstPtr&) const {
			return false;
		}

		inline virtual bool isPlayer() const = 0;
		inline virtual bool isMonster() const = 0;
		inline virtual bool isNpc() const = 0;

		int32_t getWalkDelay(Direction dir) const;
		int32_t getWalkDelay() const;
		int64_t getTimeSinceLastMove() const;

		int64_t getEventStepTicks(bool onlyDelay = false) const;
		int64_t getStepDuration(Direction dir) const;
		int64_t getStepDuration() const;
	
		virtual int32_t getStepSpeed() const {
			return getSpeed();
		}
	
		int32_t getSpeed() const {
			return baseSpeed + varSpeed;
		}
	
		void setSpeed(int32_t varSpeedDelta) {
			int32_t oldSpeed = getSpeed();
			varSpeed = varSpeedDelta;

			if (getSpeed() <= 0) {
				stopEventWalk();
				cancelNextWalk = true;
			} else if (oldSpeed <= 0 && !listWalkDir.empty()) {
				addEventWalk();
			}
		}

		void setBaseSpeed(uint32_t newBaseSpeed) {
			baseSpeed = newBaseSpeed;
		}
	
		uint32_t getBaseSpeed() const {
			return baseSpeed;
		}
	
		void setDodgeChance(uint8_t newDodgeChance) {
			dodgeChance = newDodgeChance;
		}
	
		uint8_t getDodgeChance() const {
			return dodgeChance;
		}

		void addDodgeChance(uint8_t chance) {
			if ((dodgeChance + chance) >= std::numeric_limits<uint8_t>::max()) {
				dodgeChance = std::numeric_limits<uint8_t>::max();
			} else {
				dodgeChance += chance;
			}
		}

		void removeDodgeChance(uint8_t chance) {
			if ((dodgeChance - chance) <= 0) {
				dodgeChance = 0;
			} else {
				dodgeChance -= chance;
			}
		}

		int32_t getHealth() const {
			return health;
		}
	
		virtual int32_t getMaxHealth() const {
			return healthMax;
		}
	
		bool isDead() const {
			return health <= 0;
		}
	
		void setDrunkenness(uint8_t newDrunkenness) {
			drunkenness = newDrunkenness;
		}
	
		uint8_t getDrunkenness() const {
			return drunkenness;
		}

		const Outfit_t getCurrentOutfit() const {
			return currentOutfit;
		}
	
		void setCurrentOutfit(Outfit_t outfit) {
			currentOutfit = outfit;
		}
	
		const Outfit_t getDefaultOutfit() const {
			return defaultOutfit;
		}
	
		bool isInvisible() const;
	
		ZoneType_t getZone() const {
			return getTile()->getZone();
		}

		//walk functions
		void startAutoWalk();
		void startAutoWalk(Direction direction);
		void startAutoWalk(const std::vector<Direction>& listDir);
		void addEventWalk(bool firstStep = false);
		void stopEventWalk();
		virtual void goToFollowCreature();

		//walk events
		virtual void onWalk(Direction& dir);
		virtual void onWalkAborted() {}
		virtual void onWalkComplete() {}

		//follow functions
		CreaturePtr getFollowCreature() const {
			if (auto f_target = followCreature.lock())
			{
				return f_target;
			}
			return nullptr;
		}
	
		virtual bool setFollowCreature(const CreaturePtr& creature);

		//follow events
		virtual void onFollowCreature(const CreatureConstPtr&) {}
		virtual void onFollowCreatureComplete(const CreatureConstPtr&) {}

		//combat functions
		CreaturePtr getAttackedCreature() 
		{
			if (const auto& a_target = attackedCreature.lock())
			{
				return a_target;
			}
			return nullptr;
		}
		virtual bool setAttackedCreature(const CreaturePtr& creature);
		virtual BlockType_t blockHit(const CreaturePtr& attacker, CombatType_t combatType, int32_t& damage,
		                             bool checkDefense = false, bool checkArmor = false, bool field = false, bool ignoreResistances = false);

		bool setMaster(const CreaturePtr& newMaster);

		void removeMaster() {
			if (master.lock()) {
				master.reset();
			}
		}

		bool isSummon() const {
			return master.lock() != nullptr;
		}
	
		CreaturePtr getMaster() const {
			if (auto owner = master.lock())
			{
				return owner;
			}
			return nullptr;
		}

		const std::list<CreaturePtr>& getSummons() const {
			return summons;
		}

		virtual int32_t getArmor() const {
			return 0;
		}
	
		virtual int32_t getDefense() const {
			return 0;
		}
	
		virtual float getAttackFactor() const {
			return 1.0f;
		}
	
		virtual float getDefenseFactor() const {
			return 1.0f;
		}

		virtual uint8_t getSpeechBubble() const {
			return SPEECHBUBBLE_NONE;
		}

		bool addCondition(Condition* condition, bool force = false);
		bool addCombatCondition(Condition* condition);
		void removeCondition(ConditionType_t type, ConditionId_t conditionId, bool force = false);
		void removeCondition(ConditionType_t type, bool force = false);
		void removeCondition(Condition* condition, bool force = false);
		void removeCombatCondition(ConditionType_t type);
		Condition* getCondition(ConditionType_t type) const;
		Condition* getCondition(ConditionType_t type, ConditionId_t conditionId, uint32_t subId = 0) const;
		void executeConditions(uint32_t interval);
		bool hasCondition(ConditionType_t type, uint32_t subId = 0) const;
		virtual bool isImmune(ConditionType_t type) const;
		virtual bool isImmune(CombatType_t type) const;
		virtual bool isSuppress(ConditionType_t type) const;
	
		virtual uint32_t getDamageImmunities() const {
			return 0;
		}
	
		virtual uint32_t getConditionImmunities() const {
			return 0;
		}
	
		virtual uint32_t getConditionSuppressions() const {
			return 0;
		}
	
		virtual bool isAttackable() const {
			return true;
		}

		virtual void changeHealth(int32_t healthChange, bool sendHealthChange = true);

		void gainHealth(const CreaturePtr& healer, int32_t healthGain);
		virtual void drainHealth(const CreaturePtr& attacker, int32_t damage);

		virtual bool challengeCreature(const CreaturePtr&, bool) {
			return false;
		}

		CreatureVector getKillers() const;
		void onDeath();
		virtual uint64_t getGainedExperience(const CreaturePtr& attacker) const;
		void addDamagePoints(const CreaturePtr& attacker, int32_t damagePoints);
		bool hasBeenAttacked(uint32_t attackerId);

		//combat event functions
		virtual void onAddCondition(ConditionType_t type);
		virtual void onAddCombatCondition(ConditionType_t type);
		virtual void onEndCondition(ConditionType_t type);
		void onTickCondition(ConditionType_t type, bool& bRemove);
		virtual void onCombatRemoveCondition(Condition* condition);
		virtual void onAttackedCreature(const CreaturePtr&, bool = true) {}
		virtual void onAttacked();
		virtual void onAttackedCreatureDrainHealth(const CreaturePtr& target, int32_t points);
		virtual void onTargetCreatureGainHealth(const CreaturePtr&, int32_t) {}
		virtual bool onKilledCreature(const CreaturePtr& target, bool lastHit = true);
		virtual void onGainExperience(uint64_t gainExp, const CreaturePtr& target);
		virtual void onAttackedCreatureBlockHit(BlockType_t) {}
		virtual void onBlockHit() {}
		virtual void onChangeZone(ZoneType_t zone);
		virtual void onAttackedCreatureChangeZone(ZoneType_t zone);
		virtual void onIdleStatus();

		virtual LightInfo getCreatureLight() const;
		virtual void setNormalCreatureLight();
		void setCreatureLight(LightInfo lightInfo);

		virtual void onThink(uint32_t interval);
		void onAttacking(uint32_t interval);
		virtual void onWalk();
		virtual bool getNextStep(Direction& dir, uint32_t& flags);

		void onAddTileItem(TilePtr tile, const Position& pos);
		virtual void onUpdateTileItem(const TilePtr& tile, const Position& pos, const ItemPtr& oldItem,
		                              const ItemType& oldType, const ItemPtr& newItem, const ItemType& newType);
		virtual void onRemoveTileItem(const TilePtr& tile, const Position& pos, const ItemType& iType,
		                              const ItemPtr& item);

		virtual void onCreatureAppear(const CreaturePtr& creature, bool isLogin);
		virtual void onRemoveCreature(const CreaturePtr& creature, bool isLogout);
		virtual void onCreatureMove(const CreaturePtr& creature, const TilePtr& newTile, const Position& newPos,
		                            const TilePtr& oldTile, const Position& oldPos, bool teleport);

		virtual void onAttackedCreatureDisappear(bool) {}
		virtual void onFollowCreatureDisappear(bool) {}
		virtual void onCreatureSay(const CreaturePtr&, SpeakClasses, const std::string&) {}
		virtual void onPlacedCreature() {}

		virtual bool getCombatValues(int32_t&, int32_t&) {
			return false;
		}

		size_t getSummonCount() const {
			return summons.size();
		}
	
		void setDropLoot(bool lootDrop) {
			this->lootDrop = lootDrop;
		}
	
		void setSkillLoss(bool skillLoss) {
			this->skillLoss = skillLoss;
		}
	
		void setUseDefense(bool useDefense) {
			canUseDefense = useDefense;
		}
	
		void setMovementBlocked(bool state) {
			movementBlocked = state;
			cancelNextWalk = true;
		}
	
		bool isMovementBlocked() const {
			return movementBlocked;
		}

		bool giveCustomSkill(std::string_view name, uint16_t level);
		bool giveCustomSkill(std::string_view name, std::shared_ptr<CustomSkill> new_skill);
		bool removeCustomSkill(std::string_view name);
		std::shared_ptr<CustomSkill> getCustomSkill(std::string_view name);
		const SkillRegistry& getCustomSkills() const
		{
			return c_skills;
		}

		void setCustomSkills(SkillRegistry skill_set)
		{
			c_skills = skill_set;
		}

		//creature script events
		bool registerCreatureEvent(const std::string& name);
		bool unregisterCreatureEvent(const std::string& name);

		CylinderPtr getParent() override final {
			[[likely]] if (auto shared_ptr = tile.lock())
			{
				return shared_ptr;
			}
			[[unlikely]] return nullptr;
		}

		CylinderConstPtr getParent() const override final {
			[[likely]] if (auto shared_ptr = tile.lock())
			{
				return shared_ptr;
			}
			[[unlikely]] return nullptr;
		}
	
		void setParent(std::weak_ptr<Cylinder> cylinder) override final {
			if (const auto shared_ptr = cylinder.lock())
			{
				tile = shared_ptr->getTile();
				position = shared_ptr->getPosition();
			}
		}

		const Position& getPosition() const override final {
			return position;
		}

		TilePtr getTile() override final {
			[[likely]] if (auto shared_ptr = tile.lock())
			{
				return shared_ptr;
			}
			[[unlikely]] return nullptr;
		}
	
		TileConstPtr getTile() const override final {
			[[likely]] if (auto shared_ptr = tile.lock())
			{
				return shared_ptr;
			}
			[[unlikely]] return nullptr;
		}

		int32_t getWalkCache(const Position& pos) const;

		const Position& getLastPosition() const {
			return lastPosition;
		}
	
		void setLastPosition(Position newLastPos) {
			lastPosition = newLastPos;
		}

		static bool canSee(const Position& myPos, const Position& pos, int32_t viewRangeX, int32_t viewRangeY);

		double getDamageRatio(const CreaturePtr& attacker) const;

		bool getPathTo(const Position& targetPos, std::vector<Direction>& dirList, const FindPathParams& fpp);
		bool getPathTo(const Position& targetPos, std::vector<Direction>& dirList, int32_t minTargetDist, int32_t maxTargetDist, bool fullPathSearch = true, bool clearSight = true, int32_t maxSearchDist = 0);

	protected:
		virtual bool useCacheMap() const {
			return false;
		}

		struct CountBlock_t {
			int32_t total;
			int64_t ticks;
		};

		static constexpr int32_t mapWalkWidth = Map::maxViewportX * 2 + 1;
		static constexpr int32_t mapWalkHeight = Map::maxViewportY * 2 + 1;
		static constexpr int32_t maxWalkCacheWidth = (mapWalkWidth - 1) / 2;
		static constexpr int32_t maxWalkCacheHeight = (mapWalkHeight - 1) / 2;

		Position position;

		using CountMap = std::map<uint32_t, CountBlock_t>;
		CountMap damageMap;

		std::list<CreaturePtr> summons;
		CreatureEventList eventsList;
		ConditionList conditions;

		std::vector<Direction> listWalkDir;
		SkillRegistry c_skills;

		TileWeakPtr tile;
		CreatureWeakPtr attackedCreature;
		CreatureWeakPtr master;
		CreatureWeakPtr followCreature;

		uint64_t lastStep = 0;
		uint32_t id = 0;
		uint32_t scriptEventsBitField = 0;
		uint32_t eventWalk = 0;
		uint32_t walkUpdateTicks = 0;
		uint32_t lastHitCreatureId = 0;
		uint32_t blockCount = 0;
		uint32_t blockTicks = 0;
		uint32_t lastStepCost = 1;
		uint32_t baseSpeed = 220;
		int32_t varSpeed = 0;
		int32_t health = 1000;
		int32_t healthMax = 1000;
		uint8_t drunkenness = 0;
		uint8_t dodgeChance = 0;

		Outfit_t currentOutfit;
		Outfit_t defaultOutfit;

		Position lastPosition;
		LightInfo internalLight;

		Direction direction = DIRECTION_SOUTH;
		Skulls_t skull = SKULL_NONE;

		bool localMapCache[mapWalkHeight][mapWalkWidth] = {{ false }};
		bool isInternalRemoved = false;
		bool isMapLoaded = false;
		bool isUpdatingPath = false;
		bool creatureCheck = false;
		bool inCheckCreaturesVector = false;
		bool skillLoss = true;
		bool lootDrop = true;
		bool cancelNextWalk = false;
		bool hasFollowPath = false;
		bool forceUpdateFollowPath = false;
		bool hiddenHealth = false;
		bool canUseDefense = true;
		bool movementBlocked = false;

		//creature script events
		bool hasEventRegistered(CreatureEventType_t event) const {
			return (0 != (scriptEventsBitField & (static_cast<uint32_t>(1) << event)));
		}
	
		CreatureEventList getCreatureEvents(CreatureEventType_t type) const;

		void updateMapCache();
		void updateTileCache(TilePtr tile, int32_t dx, int32_t dy);
		void updateTileCache(TilePtr tile, const Position& pos);
		void onCreatureDisappear(const CreatureConstPtr& creature, bool isLogout);
		virtual void doAttacking(uint32_t) {}
	
		virtual bool hasExtraSwing() {
			return false;
		}

		virtual uint64_t getLostExperience() const {
			return 0;
		}
	
		virtual void dropLoot(const ContainerPtr& corpse, const CreaturePtr& lastHitKiller) {}
	
		virtual uint16_t getLookCorpse() const {
			return 0;
		}
	
		virtual void getPathSearchParams(const CreatureConstPtr& creature, FindPathParams& fpp) const;
		virtual void death(const CreaturePtr&) {}
		virtual bool dropCorpse(const CreaturePtr& lastHitCreature, const CreaturePtr& mostDamageCreature, bool lastHitUnjustified, bool mostDamageUnjustified);
		virtual ItemPtr getCorpse(const CreaturePtr& lastHitCreature, const CreaturePtr& mostDamageCreature);

		friend class Game;
		friend class Map;
		friend class LuaScriptInterface;
};

#endif
