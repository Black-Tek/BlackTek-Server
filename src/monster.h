// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_MONSTER_H
#define FS_MONSTER_H

#include "tile.h"
#include "monsters.h"

class Creature;
class Game;
class Spawn;

using CreatureHashSet = std::unordered_set<CreaturePtr>;
using CreatureList = std::vector<CreatureWeakPtr>;

enum TargetSearchType_t {
	TARGETSEARCH_DEFAULT,
	TARGETSEARCH_RANDOM,
	TARGETSEARCH_ATTACKRANGE,
	TARGETSEARCH_NEAREST,
};

class Monster final : public Creature
{
	public:
		static MonsterPtr createMonster(const std::string& name);
		static int32_t despawnRange;
		static int32_t despawnRadius;

		explicit Monster(MonsterType* mType);
		~Monster() override;

		// non-copyable
		Monster(const Monster&) = delete;
		Monster& operator=(const Monster&) = delete;

		MonsterPtr getMonster() override {
			return static_shared_this<Monster>();
		}
	
		MonsterConstPtr getMonster() const override {
			return static_shared_this<const Monster>();
		}

		void setID() override {
			if (id == 0) {
				id = monsterAutoID++;
			}
		}

		void addList() override;
		void removeList() override;

		// Returns name that was used in monsters.xml, not the real creature name
		const std::string& getRegisteredName() const override { return mType->registeredName; };

		const std::string& getName() const override;
		void setName(const std::string& name);

		const std::string& getNameDescription() const override;
	
		void setNameDescription(const std::string& nameDescription) {
			this->nameDescription = nameDescription;
		};

		std::string getDescription(int32_t) const override {
			return nameDescription + '.';
		}

		bool isBoss(bool countReward = true) const
		{
			return mType->info.isBoss or ( countReward and mType->info.isRewardBoss);
		}

        CreatureType_t getType(CreaturePtr caller = nullptr) const override;

		const Position& getMasterPos() const {
			return masterPos;
		}
	
		void setMasterPos(Position pos) {
			masterPos = pos;
		}

		RaceType_t getRace() const override {
			return mType->info.race;
		}
	
		int32_t getArmor() const override {
			return mType->info.armor;
		}
	
		int32_t getDefense() const override {
			return mType->info.defense;
		}
	
		bool isPushable() const override {
			return mType->info.pushable && baseSpeed != 0;
		}
	
		bool isAttackable() const override {
			return mType->info.isAttackable;
		}

		bool canPushItems() const;
	
		bool canPushCreatures() const {
			return mType->info.canPushCreatures;
		}
	
		bool isHostile() const {
			return mType->info.isHostile;
		}
	
		bool isRewardBoss() const {
			return mType->info.isRewardBoss;
		}
	
		bool canSee(const Position& pos) const override;
	
		bool canSeeInvisibility() const override {
			return isImmune(CONDITION_INVISIBLE);
		}
	
		uint32_t getManaCost() const {
			return mType->info.manaCost;
		}
	
		void setSpawn(Spawn* spawn) {
			this->spawn = spawn;
		}
	
		bool canWalkOnFieldType(CombatType_t combatType) const;

		void onAttackedCreatureDisappear(bool isLogout) override;

		void onCreatureAppear(const CreaturePtr& creature, bool isLogin) override;
		void onRemoveCreature(const CreaturePtr& creature, bool isLogout) override;
		void onCreatureMove(const CreaturePtr& creature, const TilePtr& newTile, const Position& newPos, const TilePtr& oldTile, const Position& oldPos, bool teleport) override;
		void onCreatureSay(const CreaturePtr& creature, SpeakClasses type, const std::string& text) override;

		void drainHealth(const CreaturePtr& attacker, int32_t damage) override;
		void changeHealth(int32_t healthChange, bool sendHealthChange = true) override;

		bool isWalkingToSpawn() const {
			return walkingToSpawn;
		}
	
		bool walkToSpawn();
		void onWalk() override;
		void onWalkComplete() override;
		bool getNextStep(Direction& direction, uint32_t& flags) override;
		void onFollowCreatureComplete(const CreatureConstPtr& creature) override;

		void onThink(uint32_t interval) override;

		bool challengeCreature(const CreaturePtr& creature, bool force = false) override;

		void setNormalCreatureLight() override;
		bool getCombatValues(int32_t& min, int32_t& max) override;

		void doAttacking(uint32_t interval) override;
	
		bool hasExtraSwing() override {
			return lastMeleeAttack == 0;
		}

		bool searchTarget(TargetSearchType_t searchType = TARGETSEARCH_DEFAULT);
		bool selectTarget(const CreaturePtr& creature);

		inline bool isPlayer() const override { return false; }
		inline bool isMonster() const override { return true; }
		inline bool isNpc() const override { return false; }

		const CreatureList& getTargetList() const {
			return targetList;
		}
	
		const CreatureHashSet& getFriendList() const {
			return friendList;
		}
	
		CreatureHashSet& getFriendList() {
			return friendList;
		}

		bool isTarget(const CreatureConstPtr& creature) const;
	
		bool isFleeing() const {
			return !isSummon() && getHealth() <= mType->info.runAwayHealth && challengeFocusDuration <= 0;
		}

		void fleeFromTarget(const Position& targetPos, Direction& direction) noexcept;

        [[nodiscard]] bool followTargetFromDistance(const Position& targetPos, Direction& direction) noexcept;

	
		bool isTargetNearby() const {
			return stepDuration >= 1;
		}
	
		bool isIgnoringFieldDamage() const {
			return ignoreFieldDamage;
		}

		BlockType_t blockHit(const CreaturePtr& attacker, CombatType_t combatType, int32_t& damage,
		                     bool checkDefense = false, bool checkArmor = false, bool field = false, bool ignoreResistances = false) override;

		static uint32_t monsterAutoID;

	private:
		CreatureHashSet friendList;
		CreatureList targetList;

		std::string name;
		std::string nameDescription;

		MonsterType* mType;
		Spawn* spawn = nullptr;

		int64_t lastMeleeAttack = 0;

		uint32_t attackTicks = 0;
		uint32_t targetTicks = 0;
		uint32_t targetChangeTicks = 0;
		uint32_t defenseTicks = 0;
		uint32_t yellTicks = 0;
		int32_t minCombatValue = 0;
		int32_t maxCombatValue = 0;
		int32_t targetChangeCooldown = 0;
		int32_t challengeFocusDuration = 0;
		int32_t stepDuration = 0;

		Position masterPos;

		bool ignoreFieldDamage = false;
		bool isIdle = true;
		bool isMasterInRange = false;
		bool randomStepping = false;
		bool walkingToSpawn = false;

		void onCreatureEnter(const CreaturePtr& creature);
		void onCreatureLeave(const CreaturePtr& creature);
		void onCreatureFound(const CreaturePtr& creature, bool pushFront = false);

		void updateLookDirection();

		void addFriend(const CreaturePtr& creature);
		void removeFriend(const CreaturePtr& creature);
		void addTarget(const CreaturePtr& creature, bool pushFront = false);
		void removeTarget(const CreaturePtr& creature);

		void updateTargetList();
		void clearTargetList();
		void clearFriendList();

		void death(const CreaturePtr& lastHitCreature) override;
		ItemPtr getCorpse(const CreaturePtr& lastHitCreature, const CreaturePtr& mostDamageCreature) override;

		void setIdle(bool idle);
		void updateIdleStatus();
	
		bool getIdleStatus() const {
			return isIdle;
		}

		void onAddCondition(ConditionType_t type) override;
		void onEndCondition(ConditionType_t type) override;

		bool canUseAttack(const Position& pos, const CreatureConstPtr& target) const;
		bool canUseSpell(const Position& pos, const Position& targetPos,
		                 const spellBlock_t& sb, uint32_t interval, bool& inRange, bool& resetTicks) const;
		bool getRandomStep(const Position& creaturePos, Direction& direction);
		bool getDanceStep(const Position& creaturePos, Direction& direction,
		                  bool keepAttack = true, bool keepDistance = true);
		bool isInSpawnRange(const Position& pos) const;
		bool canWalkTo(Position pos, Direction direction);

		static bool pushItem(const ItemPtr& item);
		static void pushItems(const TilePtr& tile);
		static bool pushCreature(const CreaturePtr& creature);
		static void pushCreatures(const TilePtr& tile);

		void onThinkTarget(uint32_t interval);
		void onThinkYell(uint32_t interval);
		void onThinkDefense(uint32_t interval);

		bool isFriend(const CreatureConstPtr& creature) const;
		bool isOpponent(const CreatureConstPtr& creature) const;

		uint64_t getLostExperience() const override {
			return skillLoss ? mType->info.experience : 0;
		}
	
		uint16_t getLookCorpse() const override {
			return mType->info.lookcorpse;
		}
	
		void dropLoot(const ContainerPtr& corpse, const CreaturePtr& lastHitCreature) override;
	
		uint32_t getDamageImmunities() const override {
			return mType->info.damageImmunities;
		}
	
		uint32_t getConditionImmunities() const override {
			return mType->info.conditionImmunities;
		}
	
		void getPathSearchParams(const CreatureConstPtr& creature, FindPathParams& fpp) const override;
	
		bool useCacheMap() const override {
			return !randomStepping;
		}

		friend class LuaScriptInterface;
};

#endif
