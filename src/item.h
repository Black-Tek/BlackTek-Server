// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_ITEM_H
#define FS_ITEM_H

#include "enums.h"
#include "itemlocation.h"
#include "sharedobject.h"
#include "items.h"
#include "luascript.h"
#include "tools.h"
#include "augments.h"
#include "declarations.h"
#include "pointbasedstat.h"
#include "console.h"

#include <typeinfo>
#include <boost/variant.hpp>
#include <bitset>
#include <deque>
#include <gtl/phmap.hpp>

class Creature;
class Player;
class ItemContainer;
class Depot;
class House;
class AccessList;

using namespace Components::Stats;

enum ITEMPROPERTY {
	CONST_PROP_BLOCKSOLID = 0,
	CONST_PROP_HASHEIGHT,
	CONST_PROP_BLOCKPROJECTILE,
	CONST_PROP_BLOCKPATH,
	CONST_PROP_ISVERTICAL,
	CONST_PROP_ISHORIZONTAL,
	CONST_PROP_MOVEABLE,
	CONST_PROP_IMMOVABLEBLOCKSOLID,
	CONST_PROP_IMMOVABLEBLOCKPATH,
	CONST_PROP_IMMOVABLENOFIELDBLOCKPATH,
	CONST_PROP_NOFIELDBLOCKPATH,
	CONST_PROP_SUPPORTHANGABLE,
};

enum TradeEvents_t {
	ON_TRADE_TRANSFER,
	ON_TRADE_CANCEL,
};

enum ItemDecayState_t : uint8_t {
	DECAYING_FALSE = 0,
	DECAYING_TRUE,
	DECAYING_PENDING,
};

enum AttrTypes_t {
	//ATTR_DESCRIPTION = 1,
	//ATTR_EXT_FILE = 2,
	ATTR_TILE_FLAGS = 3,
	ATTR_ACTION_ID = 4,
	ATTR_UNIQUE_ID = 5,
	ATTR_TEXT = 6,
	ATTR_DESC = 7,
	ATTR_TELE_DEST = 8,
	ATTR_ITEM = 9,
	ATTR_DEPOT_ID = 10,
	//ATTR_EXT_SPAWN_FILE = 11,
	ATTR_RUNE_CHARGES = 12,
	//ATTR_EXT_HOUSE_FILE = 13,
	ATTR_HOUSEDOORID = 14,
	ATTR_COUNT = 15,
	ATTR_DURATION = 16,
	ATTR_DECAYING_STATE = 17,
	ATTR_WRITTENDATE = 18,
	ATTR_WRITTENBY = 19,
	ATTR_SLEEPERGUID = 20,
	ATTR_SLEEPSTART = 21,
	ATTR_CHARGES = 22,
	ATTR_CONTAINER_ITEMS = 23,
	ATTR_NAME = 24,
	ATTR_ARTICLE = 25,
	ATTR_PLURALNAME = 26,
	ATTR_WEIGHT = 27,
	ATTR_ATTACK = 28,
	ATTR_DEFENSE = 29,
	ATTR_EXTRADEFENSE = 30,
	ATTR_ARMOR = 31,
	ATTR_HITCHANCE = 32,
	ATTR_SHOOTRANGE = 33,
	ATTR_CUSTOM_ATTRIBUTES = 34,
	ATTR_DECAYTO = 35,
	ATTR_WRAPID = 36,
	ATTR_STOREITEM = 37,
	ATTR_ATTACK_SPEED = 38,
	ATTR_PLACE_HOLDER = 39,
	ATTR_PLACE_HOLDERTOO = 40,
	ATTR_CLASSIFICATION = 41,
	ATTR_TIER = 42,
	ATTR_IMBUESLOTS = 43,  // legacy — values are skipped during load for backward compat
	ATTR_IMBUEMENTS = 44,  // legacy — values are skipped during load for backward compat
	ATTR_REWARDID = 45,
	ATTR_OPENCONTAINER = 46
};

enum Attr_ReadValue {
	ATTR_READ_CONTINUE,
	ATTR_READ_ERROR,
	ATTR_READ_END,
};

class ItemAttributes
{
	public:
		ItemAttributes() = default;

		void setSpecialDescription(const std::string& desc) {
			setStrAttr(ITEM_ATTRIBUTE_DESCRIPTION, desc);
		}
	
		const std::string& getSpecialDescription() const {
			return getStrAttr(ITEM_ATTRIBUTE_DESCRIPTION);
		}

		void setText(const std::string& text) {
			setStrAttr(ITEM_ATTRIBUTE_TEXT, text);
		}
	
		void resetText() {
			removeAttribute(ITEM_ATTRIBUTE_TEXT);
		}
	
		const std::string& getText() const {
			return getStrAttr(ITEM_ATTRIBUTE_TEXT);
		}

		void setDate(int32_t n) {
			setIntAttr(ITEM_ATTRIBUTE_DATE, n);
		}
	
		void resetDate() {
			removeAttribute(ITEM_ATTRIBUTE_DATE);
		}
	
		time_t getDate() const {
			return static_cast<time_t>(getIntAttr(ITEM_ATTRIBUTE_DATE));
		}

		void setWriter(const std::string& writer) {
			setStrAttr(ITEM_ATTRIBUTE_WRITER, writer);
		}
	
		void resetWriter() {
			removeAttribute(ITEM_ATTRIBUTE_WRITER);
		}
	
		const std::string& getWriter() const {
			return getStrAttr(ITEM_ATTRIBUTE_WRITER);
		}

		void setActionId(uint16_t n) {
			setIntAttr(ITEM_ATTRIBUTE_ACTIONID, n);
		}
	
		uint16_t getActionId() const {
			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_ACTIONID));
		}

		void setUniqueId(uint16_t n) {
			setIntAttr(ITEM_ATTRIBUTE_UNIQUEID, n);
		}
	
		uint16_t getUniqueId() const {
			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_UNIQUEID));
		}

		void setCharges(uint16_t n) {
			setIntAttr(ITEM_ATTRIBUTE_CHARGES, n);
		}
	
		uint16_t getCharges() const {
			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_CHARGES));
		}

		void setFluidType(uint16_t n) {
			setIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE, n);
		}
	
		uint16_t getFluidType() const {
			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE));
		}

		void setOwner(uint32_t owner) {
			setIntAttr(ITEM_ATTRIBUTE_OWNER, owner);
		}
	
		uint32_t getOwner() const {
			return getIntAttr(ITEM_ATTRIBUTE_OWNER);
		}

		void setCorpseOwner(uint32_t corpseOwner) {
			setIntAttr(ITEM_ATTRIBUTE_CORPSEOWNER, corpseOwner);
		}
	
		uint32_t getCorpseOwner() const {
			return getIntAttr(ITEM_ATTRIBUTE_CORPSEOWNER);
		}

		void setDuration(int32_t time) {
			setIntAttr(ITEM_ATTRIBUTE_DURATION, time);
		}
	
		void decreaseDuration(int32_t time) {
			increaseIntAttr(ITEM_ATTRIBUTE_DURATION, -time);
		}
	
		uint32_t getDuration() const {
			return getIntAttr(ITEM_ATTRIBUTE_DURATION);
		}

		void setDecaying(ItemDecayState_t decayState) {
			setIntAttr(ITEM_ATTRIBUTE_DECAYSTATE, decayState);
		}

		ItemDecayState_t getDecaying() const {
			return static_cast<ItemDecayState_t>(getIntAttr(ITEM_ATTRIBUTE_DECAYSTATE));
		}

		struct CustomAttribute
		{
			typedef boost::variant<boost::blank, std::string, int64_t, double, bool> VariantAttribute;
			VariantAttribute value;

			CustomAttribute() : value(boost::blank()) {}

			bool operator==(const CustomAttribute& otherAttr) const {
				return value == otherAttr.value;
			}

			bool operator!=(const CustomAttribute& otherAttr) const {
				return value != otherAttr.value;
			}

			template<typename T>
			explicit CustomAttribute(const T& v) : value(v) {}

			template<typename T>
			void set(const T& v) {
				value = v;
			}

			template<typename T>
			const T& get();

			struct PushLuaVisitor : public boost::static_visitor<> {
				lua_State* L;

				explicit PushLuaVisitor(lua_State* L) : boost::static_visitor<>(), L(L) {}

				void operator()(const boost::blank&) const {
					lua_pushnil(L);
				}

				void operator()(const std::string& v) const {
					LuaScriptInterface::pushString(L, v);
				}

				void operator()(bool v) const {
					LuaScriptInterface::pushBoolean(L, v);
				}

				void operator()(const int64_t& v) const {
					lua_pushinteger(L, v);
				}

				void operator()(const double& v) const {
					lua_pushnumber(L, v);
				}
			};

			void pushToLua(lua_State* L) const {
				boost::apply_visitor(PushLuaVisitor(L), value);
			}

			struct SerializeVisitor : public boost::static_visitor<> {
				PropWriteStream& propWriteStream;

				explicit SerializeVisitor(PropWriteStream& propWriteStream) : boost::static_visitor<>(), propWriteStream(propWriteStream) {}

				void operator()(const boost::blank&) const {
				}

				void operator()(const std::string& v) const {
					propWriteStream.writeString(v);
				}

				template<typename T>
				void operator()(const T& v) const {
					propWriteStream.write<T>(v);
				}
			};

			void serialize(PropWriteStream& propWriteStream) const {
				propWriteStream.write<uint8_t>(static_cast<uint8_t>(value.which()));
				boost::apply_visitor(SerializeVisitor(propWriteStream), value);
			}

			bool unserialize(PropStream& propStream) {
				// This is hard-coded so it's not general, depends on the position of the variants.
				uint8_t pos;
				if (!propStream.read<uint8_t>(pos)) {
					return false;
				}

				switch (pos) {
					case 1:  { // std::string
						auto [str, ok] = propStream.readString();
						if (!ok) {
							return false;
						}
						value = std::string{ str };
						break;
					}

					case 2: { // int64_t
						int64_t tmp;
						if (!propStream.read<int64_t>(tmp)) {
							return false;
						}
						value = tmp;
						break;
					}

					case 3: { // double
						double tmp;
						if (!propStream.read<double>(tmp)) {
							return false;
						}
						value = tmp;
						break;
					}

					case 4: { // bool
						bool tmp;
						if (!propStream.read<bool>(tmp)) {
							return false;
						}
						value = tmp;
						break;
					}

					default: {
						value = boost::blank();
						return false;
					}
				}
				return true;
			}
		};

	private:
		bool hasAttribute(itemAttrTypes type) const {
			return (type & attributeBits) != 0;
		}
		void removeAttribute(itemAttrTypes type);

		static std::string emptyString;
		static int64_t emptyInt;
		static double emptyDouble;
		static bool emptyBool;

		typedef gtl::node_hash_map<std::string, CustomAttribute> CustomAttributeMap;

		struct Attribute
		{
			union {
				int64_t integer;
				std::string* string;
				CustomAttributeMap* custom;
			} value;
			itemAttrTypes type;

			explicit Attribute(itemAttrTypes type) : type(type) {
				memset(&value, 0, sizeof(value));
			}
			Attribute(const Attribute& i) {
				type = i.type;
				if (ItemAttributes::isIntAttrType(type)) {
					value.integer = i.value.integer;
				} else if (ItemAttributes::isStrAttrType(type)) {
					value.string = new std::string(*i.value.string);
				} else if (ItemAttributes::isCustomAttrType(type)) {
					value.custom = new CustomAttributeMap(*i.value.custom);
				} else {
					memset(&value, 0, sizeof(value));
				}
			}
			
			Attribute(Attribute&& attribute) : value(attribute.value), type(attribute.type) {
				memset(&attribute.value, 0, sizeof(value));
				attribute.type = ITEM_ATTRIBUTE_NONE;
			}
			
			~Attribute() {
				if (ItemAttributes::isStrAttrType(type)) {
					delete value.string;
				} else if (ItemAttributes::isCustomAttrType(type)) {
					delete value.custom;
				}
			}
			
			Attribute& operator=(Attribute other) {
				Attribute::swap(*this, other);
				return *this;
			}
			
			Attribute& operator=(Attribute&& other) {
				if (this != &other) {
					if (ItemAttributes::isStrAttrType(type)) {
						delete value.string;
					} else if (ItemAttributes::isCustomAttrType(type)) {
						delete value.custom;
					}

					value = other.value;
					type = other.type;

					memset(&other.value, 0, sizeof(value));
					other.type = ITEM_ATTRIBUTE_NONE;
				}
				return *this;
			}

			static void swap(Attribute& first, Attribute& second) {
				std::swap(first.value, second.value);
				std::swap(first.type, second.type);
			}
		};

		std::vector<Attribute> attributes;
		uint32_t attributeBits = 0;

		const std::string& getStrAttr(itemAttrTypes type) const;
		void setStrAttr(itemAttrTypes type, std::string_view value);

		int64_t getIntAttr(itemAttrTypes type) const;
		void setIntAttr(itemAttrTypes type, int64_t value);
		void increaseIntAttr(itemAttrTypes type, int64_t value);

		const Attribute* getExistingAttr(itemAttrTypes type) const;
		Attribute& getAttr(itemAttrTypes type);

		CustomAttributeMap* getCustomAttributeMap() {
			if (!hasAttribute(ITEM_ATTRIBUTE_CUSTOM)) {
				return nullptr;
			}

			return getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom;
		}

		template<typename R>
		void setCustomAttribute(int64_t key, R value) {
			auto tmp = std::to_string(key);
			setCustomAttribute(tmp, value);
		}

		void setCustomAttribute(int64_t key, CustomAttribute& value) {
			auto tmp = std::to_string(key);
			setCustomAttribute(tmp, value);
		}

		template<typename R>
		void setCustomAttribute(std::string_view key, R value)
		{

			if (hasAttribute(ITEM_ATTRIBUTE_CUSTOM)) {
				removeCustomAttribute(key);
			} else {
				getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom = new CustomAttributeMap();
			}
			auto lowercaseKey = boost::algorithm::to_lower_copy(std::string{ key });
			getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom->emplace(lowercaseKey, value);
		}

		void setCustomAttribute(std::string_view key, const CustomAttribute& value) {

			if (hasAttribute(ITEM_ATTRIBUTE_CUSTOM)) {
				removeCustomAttribute(key);
			} else {
				getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom = new CustomAttributeMap();
			}
			auto lowercaseKey = boost::algorithm::to_lower_copy(std::string{ key });
			getAttr(ITEM_ATTRIBUTE_CUSTOM).value.custom->emplace(lowercaseKey, value);
		}

		const CustomAttribute* getCustomAttribute(int64_t key) {
			auto tmp = std::to_string(key);
			return getCustomAttribute(tmp);
		}

		const CustomAttribute* getCustomAttribute(std::string_view key) 
		{
			if (const CustomAttributeMap* customAttrMap = getCustomAttributeMap()) {
				auto lowercaseKey = boost::algorithm::to_lower_copy(std::string{ key });
				if (auto it = customAttrMap->find(lowercaseKey); it != customAttrMap->end()) {
					return &(it->second);
				}
			}
			return nullptr;
		}

		bool removeCustomAttribute(int64_t key) {
			auto tmp = std::to_string(key);
			return removeCustomAttribute(tmp);
		}

		bool removeCustomAttribute(std::string_view key) {
			if (CustomAttributeMap* customAttrMap = getCustomAttributeMap()) {
				auto lowercaseKey = boost::algorithm::to_lower_copy(std::string{ key });
				if (auto it = customAttrMap->find(lowercaseKey); it != customAttrMap->end()) {
					customAttrMap->erase(it);

					//if (customAttrMap->empty()) {
					//	removeAttribute(ITEM_ATTRIBUTE_CUSTOM);
					//}
					return true;
				}
			}
			return false;
		}

		static constexpr uint32_t intAttributeTypes = ITEM_ATTRIBUTE_ACTIONID | ITEM_ATTRIBUTE_UNIQUEID | ITEM_ATTRIBUTE_DATE
			| ITEM_ATTRIBUTE_WEIGHT | ITEM_ATTRIBUTE_ATTACK | ITEM_ATTRIBUTE_DEFENSE | ITEM_ATTRIBUTE_EXTRADEFENSE
			| ITEM_ATTRIBUTE_ARMOR | ITEM_ATTRIBUTE_HITCHANCE | ITEM_ATTRIBUTE_SHOOTRANGE | ITEM_ATTRIBUTE_OWNER
			| ITEM_ATTRIBUTE_DURATION | ITEM_ATTRIBUTE_DECAYSTATE | ITEM_ATTRIBUTE_CORPSEOWNER | ITEM_ATTRIBUTE_CHARGES
			| ITEM_ATTRIBUTE_FLUIDTYPE | ITEM_ATTRIBUTE_DOORID | ITEM_ATTRIBUTE_DECAYTO | ITEM_ATTRIBUTE_WRAPID | ITEM_ATTRIBUTE_STOREITEM
			| ITEM_ATTRIBUTE_ATTACK_SPEED | ITEM_ATTRIBUTE_REWARDID | ITEM_ATTRIBUTE_OPENCONTAINER;
		static constexpr uint32_t stringAttributeTypes = ITEM_ATTRIBUTE_DESCRIPTION | ITEM_ATTRIBUTE_TEXT | ITEM_ATTRIBUTE_WRITER
			| ITEM_ATTRIBUTE_NAME | ITEM_ATTRIBUTE_ARTICLE | ITEM_ATTRIBUTE_PLURALNAME | ITEM_ATTRIBUTE_CLASSIFICATION | ITEM_ATTRIBUTE_TIER;

	public:
		static bool isIntAttrType(itemAttrTypes type) {
			return (type & intAttributeTypes) == type;
		}
		static bool isStrAttrType(itemAttrTypes type) {
			return (type & stringAttributeTypes) == type;
		}
		inline static bool isCustomAttrType(itemAttrTypes type) {
			return (type & ITEM_ATTRIBUTE_CUSTOM) == type;
		}

		const std::vector<Attribute>& getList() const {
			return attributes;
		}

	friend class Item;
};

enum class ItemSubType : uint8_t
{
	None,
	Container,
	DepotChest,
	DepotLocker,
	Inbox,
	StoreInbox,
	RewardChest,
	Teleport,
	TrashHolder,
	Mailbox,
	Door,
	MagicField,
	BedItem,
	HouseTransferItem,
};

enum class ContainerSubType : uint8_t
{
	None,
	DepotChest,
	DepotLocker,
	Inbox,
	StoreInbox,
	RewardChest,
};

namespace BlackTek::Containers
{
	// Composable behavior flags for ItemContainer, keyed off ContainerSubType via
	// ConfigFor() in itemcontainer.cpp. See container_properties.md for the full
	// audit and rationale behind each flag.
	enum class ContainerProperty : uint8_t
	{
		Removable,							// the container-item itself can be removed from its parent
		NeverAcceptsDirectInsert,
		RequiresNoLimitFlagToAccept,
		RejectsPlayerInitiatedInserts,
		RequiresStoreItem,
		AcceptsStoreItemsDirectly,			// exempt from the generic "store items can't move here" rejection
		BlocksNestedInsertWithoutNoLimit,
		BlocksInsertIntoStoreChild,
		EnforcesItemCountLimit,				// maxDepotItems cap via getItemHoldingCount(), in addition to slot capacity
		SkipsOwnNodeInChain,				// parent-chain walks (getParent, ancestor-walk, "move up") pass through this node
		NotifiesViaParentWalk,
		NotifiesViaOwnerParent,
		NotifyLinkIsTopParent,				// when NotifiesViaOwnerParent is set, use LINK_TOPPARENT instead of LINK_PARENT
		Count								// sentinel for bitset sizing
	};

	class ContainerConfig
	{
		public:
			constexpr ContainerConfig() = default;

			constexpr ContainerConfig& Set(ContainerProperty property)
			{
				bits.set(static_cast<size_t>(property));
				return *this;
			}

			constexpr bool Has(ContainerProperty property) const
			{
				return bits.test(static_cast<size_t>(property));
			}

		private:
			std::bitset<static_cast<size_t>(ContainerProperty::Count)> bits;
	};
}

class Item : public SharedObject
{
	public:
		// Constructor for items
		explicit Item(const uint16_t type, uint16_t count = 0);
		Item(const Item& i);
		virtual ~Item();

		// non-assignable
		Item& operator=(const Item&) = delete;

		// static
		static Items items;

		//Factory member to create item of right type based on type
		static ItemPtr CreateItem(const uint16_t type, uint16_t count = 0);
		static ItemPtr CreateContainerItem(const uint16_t type, uint16_t size);
		static ItemPtr CreateItem(PropStream& propStream);
		static ItemPtr createHouseTransferItem(House* house);
		static std::string getDescription(const ItemType& it, int32_t lookDistance, const ItemPtr& item = nullptr, int32_t subType = -1, bool addArticle = true);
		static std::string getNameDescription(const ItemType& it, const ItemConstPtr& item = nullptr, int32_t subType = -1, bool addArticle = true);
		static std::string getWeightDescription(const ItemType& it, uint32_t weight, uint32_t count = 1);

		static uint32_t countByType(const ItemConstPtr& i, int32_t subType)
		{
			if (subType == -1 or subType == i->getSubType())
				return i->getItemCount();

			return 0;
		}

		// strings declarations
		std::string getDescription(int32_t lookDistance);
		std::string getNameDescription() const;
		std::string getWeightDescription() const;

		// strings inline
		// move to cpp
		const std::string& getStrAttr(itemAttrTypes type) const
		{
			if (not attributes)
				return ItemAttributes::emptyString;

			return attributes->getStrAttr(type);
		}

		const std::string& getSpecialDescription() const	{ return getStrAttr(ITEM_ATTRIBUTE_DESCRIPTION); }
		const std::string& getText() const					{ return getStrAttr(ITEM_ATTRIBUTE_TEXT); }
		const std::string& getWriter() const				{ return getStrAttr(ITEM_ATTRIBUTE_WRITER); }

		// move to cpp
		const std::string& getName() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_NAME))
				return getStrAttr(ITEM_ATTRIBUTE_NAME);

			return items[id].name;
		}

		// move to cpp
		const std::string getPluralName() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_PLURALNAME))
				return getStrAttr(ITEM_ATTRIBUTE_PLURALNAME);

			return items[id].getPluralName();
		}

		// move to cpp
		const std::string& getArticle() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_ARTICLE))
				return getStrAttr(ITEM_ATTRIBUTE_ARTICLE);

			return items[id].article;
		}

		// move to cpp
		const std::string getClassification() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_CLASSIFICATION))
				return getStrAttr(ITEM_ATTRIBUTE_CLASSIFICATION);

			return items[id].classification;
		}

		// move to cpp
		const std::string getTier() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_TIER))
				return getStrAttr(ITEM_ATTRIBUTE_TIER);

			return items[id].tier;
		}

		// pointers declarations
		std::shared_ptr<CustomSkill> getCustomSkill(std::string_view name);
		ItemPtr				clone() const;
		TeleportPtr			getTeleport();
		TeleportConstPtr	getTeleport() const;
		TrashHolderPtr		getTrashHolder();
		TrashHolderConstPtr	getTrashHolder() const;
		MailboxPtr			getMailbox();
		MailboxConstPtr		getMailbox() const;
		DoorPtr				getDoor();
		DoorConstPtr		getDoor() const;
		MagicFieldPtr		getMagicField();
		MagicFieldConstPtr	getMagicField() const;
		BedItemPtr			getBed();
		BedItemConstPtr		getBed() const;
		BedItemPtr			getNextBedItem() const;
		PlayerPtr			getHoldingPlayer();
		ContainerPtr		getContainer();
		ContainerConstPtr	getContainer() const;
		TilePtr				getTile();
		std::shared_ptr<const Tile> getTile() const;

		// pointers inline
		ItemPtr			getItem()		{ return static_shared_this<Item>(); }
		ItemConstPtr	getItem() const	{ return static_shared_this<const Item>(); }

		// move to cpp
		std::unique_ptr<ItemAttributes>& getAttributes()
		{
			if (not attributes)
				attributes.reset(new ItemAttributes());

			return attributes;
		}

		ItemPtr getContainerParent() const { return containerParent.lock(); }

		// move to cpp
		const Components::Stats::StandardStatPtr getCustomStat(uint32_t id)
		{
			if (not c_stats.get())
				return nullptr;

			auto it = c_stats.get()->find(id);
			if (it != c_stats.get()->end())
				return it->second;

			return nullptr;
		}

		// ItemLocation
		[[nodiscard]] BlackTek::ItemLocation getLocation();

		// raw pointers
		House* getHouse() const { return house; }

		// move to cpp
		const ItemAttributes::CustomAttribute* getCustomAttribute(int64_t key)
		{
			if (not attributes)
				return nullptr;

			return getAttributes()->getCustomAttribute(key);
		}

		// move to cpp
		const ItemAttributes::CustomAttribute* getCustomAttribute(const std::string& key)
		{
			if (not attributes)
				return nullptr;

			return getAttributes()->getCustomAttribute(key);
		}

		// enums declarations
		//serialization
		Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream);
		Attr_ReadValue readGenericAttr(AttrTypes_t attr, PropStream& propStream);

		// enums inline
		ItemSubType getItemSubType() const { return item_subtype; }
		ItemDecayState_t		getDecaying() const
		{
			if (not attributes)
				return DECAYING_FALSE;

			return static_cast<ItemDecayState_t>(getIntAttr(ITEM_ATTRIBUTE_DECAYSTATE));
		}

		WeaponType_t	getWeaponType() const	{ return items[id].weaponType; }
		Ammo_t			getAmmoType() const		{ return items[id].ammoType; }

		CombatType_t getCombatType() const
		{
			const ItemType& it = items[getID()];
			return it.combatType;
		}

		// int64
		// move to cpp
		int64_t getIntAttr(itemAttrTypes type) const
		{
			if (not attributes)
				return 0;

			return attributes->getIntAttr(type);
		}

		// int32 declarations
		int32_t getDamage() const;

		// int32 inline
		// move to cpp
		int32_t getDecayTime() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_DURATION))
				return getIntAttr(ITEM_ATTRIBUTE_DURATION);

			return items[id].decayTime;
		}

		// move to cpp
		int32_t getDecayTo() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_DECAYTO))
				return getIntAttr(ITEM_ATTRIBUTE_DECAYTO);

			return items[id].decayTo;
		}

		int32_t getThrowRange() const { return (isPickupable() ? 15 : 2); }

		// move to cpp
		int32_t getAttack() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_ATTACK))
				return getIntAttr(ITEM_ATTRIBUTE_ATTACK);

			return items[id].attack;
		}

		// move to cpp
		int32_t getArmor() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_ARMOR))
				return getIntAttr(ITEM_ATTRIBUTE_ARMOR);

			return items[id].armor;
		}

		// move to cpp
		int32_t getDefense() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_DEFENSE))
				return getIntAttr(ITEM_ATTRIBUTE_DEFENSE);

			return items[id].defense;
		}

		// move to cpp
		int32_t getExtraDefense() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_EXTRADEFENSE))
				return getIntAttr(ITEM_ATTRIBUTE_EXTRADEFENSE);

			return items[id].extraDefense;
		}

		int32_t getSlotPosition() const { return items[id].slotPosition; }

		// uint32 declarations
		uint32_t getWeight() const;
		uint32_t getWorth() const;

		// uint32 inline
		uint32_t getDoorId() const	{ return getIntAttr(ITEM_ATTRIBUTE_DOORID); }
		uint32_t getSleeper() const	{ return sleeperGUID; }

		// move to cpp
		uint32_t getOwner() const
		{
			if (not attributes)
				return 0;

			return getIntAttr(ITEM_ATTRIBUTE_OWNER);
		}

		// move to cpp
		uint32_t getCorpseOwner() const
		{
			if (not attributes)
				return 0;

			return getIntAttr(ITEM_ATTRIBUTE_CORPSEOWNER);
		}

		// move to cpp
		uint32_t getDuration() const
		{
			if (not attributes)
				return 0;

			return getIntAttr(ITEM_ATTRIBUTE_DURATION);
		}

		// move to cpp
		uint32_t getBaseWeight() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_WEIGHT))
				return getIntAttr(ITEM_ATTRIBUTE_WEIGHT);

			return items[id].weight;
		}

		// move to cpp
		uint32_t getAttackSpeed() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_ATTACK_SPEED))
				return getIntAttr(ITEM_ATTRIBUTE_ATTACK_SPEED);

			return items[id].attackSpeed;
		}

		uint32_t getDefaultDuration() const { return items[id].decayTime * 1000; }

		[[nodiscard]] uint32_t getAttackModifierCount() const noexcept		{ return attack_modifier_count; }
		[[nodiscard]] uint32_t getDefenseModifierCount() const noexcept	{ return defense_modifier_count; }
		[[nodiscard]] uint32_t getConversionModifierCount() const noexcept	{ return conversion_modifier_count; }
		[[nodiscard]] uint32_t getReformModifierCount() const noexcept		{ return reform_modifier_count; }

		// uint16 declarations
		uint16_t getSubType() const;

		// uint16 inline
		// move to cpp
		uint16_t getActionId() const
		{
			if (not attributes)
				return 0;

			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_ACTIONID));
		}

		// move to cpp
		uint16_t getUniqueId() const
		{
			if (not attributes)
				return 0;

			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_UNIQUEID));
		}

		// move to cpp
		uint16_t getCharges() const
		{
			if (not attributes)
				return 0;

			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_CHARGES));
		}

		// move to cpp
		uint16_t getFluidType() const
		{
			if (not attributes)
				return 0;

			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE));
		}

		// this id, is not item's id. This is itemtype's id basically (what you see in e.g. items.xml or dat tools), so do not be misleaded.
		// the good course of action, would be to rename this method to:
		// getItemTypeID(), setItemTypeID(), id -> itemTypeID.
		// todo: think about it, but be careful as itemType also has `getID` and `setID`. Do not just `replace all`.
		uint16_t getID() const			{ return id; }
		uint16_t getEquipSlot() const	{ return items[id].equipSlot; }
		// get the number of items
		uint16_t getItemCount() const	{ return count; }

		// uint8
		// move to cpp
		uint8_t getShootRange() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_SHOOTRANGE))
				return getIntAttr(ITEM_ATTRIBUTE_SHOOTRANGE);

			return items[id].shootRange;
		}

		// int8
		// move to cpp
		int8_t getHitChance() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_HITCHANCE))
				return getIntAttr(ITEM_ATTRIBUTE_HITCHANCE);

			return items[id].hitChance;
		}

		// time_t
		time_t getDate() const { return static_cast<time_t>(getIntAttr(ITEM_ATTRIBUTE_DATE)); }

		// Position declarations
		const Position& getPosition() const;

		// Position inline
		const Position& getDestPos() const { return teleportDestPos; }

		// LightInfo
		LightInfo getLightInfo() const;

		// containers
		const gtl::flat_hash_map<std::string, std::shared_ptr<CustomSkill>>& getCustomSkills() const
		{
			return custom_skills;
		}

		std::unique_ptr<std::vector<std::shared_ptr<BlackTek::Augment>>>& getAugments()
		{
			if (not augments.get())
			{
				augments = std::make_unique<std::vector<std::shared_ptr<BlackTek::Augment>>>();
			}

			return augments;
		}

		// move to cpp
		const Components::Stats::StatRegistry& getCustomStats()
		{
			if (not c_stats.get())
				c_stats = std::make_unique<Components::Stats::StatRegistry>();

			return *c_stats;
		}

		// bool declarations
		bool		giveCustomSkill(std::string_view name, uint16_t level);
		bool		giveCustomSkill(std::string_view name, std::shared_ptr<CustomSkill> new_skill);
		bool		removeCustomSkill(std::string_view name);
		bool		equals(const ItemConstPtr& otherItem) const;
		bool		canUse(const PlayerConstPtr& player) const;
		bool		getAccessList(std::string& list) const;
		bool		trySleep(const PlayerPtr& player);
		bool		sleep(const PlayerPtr& player);
		const bool	isEquipped();
		bool		unserializeAttr(PropStream& propStream);
		bool		unserializeAugments(PropStream& propStream);
		bool		unserializeItemNode(OTB::Loader&, const OTB::Node&, PropStream& propStream);
		bool		hasProperty(ITEMPROPERTY prop) const;
		bool		canDecay();
		bool		canRemove() const;
		bool		canTransform() const;
		bool		hasMarketAttributes() const;
		bool		isRemoved() const;
		const bool	addAugment(std::string_view augmentName);
		const bool	addAugment(const std::shared_ptr<BlackTek::Augment>& augment);
		const bool	removeAugment(std::string_view name);
		const bool	removeAugment(std::shared_ptr<BlackTek::Augment>& augment);
		bool		isAugmented() const;
		bool		hasAugment(std::string_view name) const;
		bool		hasAugment(const std::shared_ptr<BlackTek::Augment>& augment) const;

		// bool inline
		bool isReplaceable() const	{ return Item::items[getID()].replaceable; }
		bool isPushable() const		{ return isMoveable(); }

		// move to cpp
		bool hasAttribute(itemAttrTypes type) const
		{
			if (not attributes)
				return false;

			return attributes->hasAttribute(type);
		}

		// move to cpp
		bool removeCustomAttribute(int64_t key)
		{
			if (not attributes)
				return false;

			return getAttributes()->removeCustomAttribute(key);
		}

		// move to cpp
		bool removeCustomAttribute(std::string_view key)
		{
			if (not attributes)
				return false;

			return getAttributes()->removeCustomAttribute(key);
		}

		bool isBlocking() const		{ return items[id].blockSolid; }
		bool isStackable() const	{ return items[id].stackable; }
		bool isAlwaysOnTop() const	{ return items[id].alwaysOnTop; }
		bool isGroundTile() const	{ return items[id].isGroundTile(); }
		bool isMagicField() const	{ return items[id].isMagicField(); }
		bool isMoveable() const		{ return items[id].moveable; }
		bool isPickupable() const	{ return items[id].pickupable; }
		bool isUseable() const		{ return items[id].useable; }
		bool isHangable() const		{ return items[id].isHangable; }

		bool isRotatable() const
		{
			const ItemType& it = items[id];
			return it.rotatable and it.rotateTo;
		}

		bool hasWalkStack() const { return items[id].walkStack; }

		// move to cpp
		bool isStoreItem() const
		{
			if (hasAttribute(ITEM_ATTRIBUTE_STOREITEM))
				return getIntAttr(ITEM_ATTRIBUTE_STOREITEM) == 1;

			return items[id].storeItem;
		}

		bool isLoadedFromMap() const { return loadedFromMap; }

		bool isCleanable() const
		{
			return not loadedFromMap and canRemove() and isPickupable() and not hasAttribute(ITEM_ATTRIBUTE_UNIQUEID) and not hasAttribute(ITEM_ATTRIBUTE_ACTIONID);
		}

		[[nodiscard]] bool isScriptDetached() const noexcept { return scriptDetached; }

		// move to cpp
		bool giveCustomStat(uint32_t id, uint32_t max_points, uint32_t current_points = 0)
		{
			if (not c_stats.get())
				c_stats = std::make_unique<StatRegistry>();

			auto stat = std::make_shared<StandardStat>(id, current_points, max_points);
			return c_stats.get()->try_emplace(id, stat).second;
		}

		// move to cpp
		bool giveCustomStat(uint32_t id, StandardStatPtr new_stat)
		{
			if (not c_stats.get())
				c_stats = std::make_unique<StatRegistry>();

			return c_stats.get()->try_emplace(id, new_stat).second;
		}

		// move to cpp
		bool removeCustomStat(uint32_t id)
		{
			if (auto stats = c_stats.get())
			{
				return stats->erase(id) > 0;
			}
			return false;
		}

		// move to cpp
		bool increaseCustomStat(uint32_t id, uint32_t amount)
		{
			if (not c_stats.get())
				return false;

			auto& stat = c_stats.get()->find(id)->second;
			return stat->add(amount);
		}

		// move to cpp
		bool decreaseCustomStat(uint32_t id, uint32_t amount)
		{
			if (not c_stats.get())
				return false;

			auto& stat = c_stats.get()->find(id)->second;
			return stat->remove(amount);
		}

		bool hasCustomStats() { return c_stats.get() != nullptr; }

		// move to cpp
		bool hasCustomStat(uint32_t id)
		{
			if (not c_stats.get())
				return false;

			auto result = c_stats.get()->find(id);
			return result != c_stats.get()->end();
		}

		// void declarations
		void teleportItem(const ItemPtr& item);
		void teleportCreature(const CreaturePtr& creature);
		void consumeItem(const ItemPtr& item);
		void deliverItem(const ItemPtr& item);
		void setAccessList(std::string_view textlist);
		void setHouse(House* house);
		void onStepInField(const CreaturePtr& creature);
		void updateHouse();
		void wakeUp(const PlayerPtr& player);
		void serializeAttr(PropWriteStream& propWriteStream) const;
		void serializeGenericAttr(PropWriteStream& propWriteStream) const;
		void setID(uint16_t newid);
		void setDefaultSubtype();
		void setSubType(uint16_t n);
		void setUniqueId(uint16_t n);
		void onRemoved();
		void onTradeEvent(TradeEvents_t event, const PlayerPtr& owner);
		void startDecaying();
		void attachContainer(uint16_t size, bool unlocked, bool pagination, ContainerSubType subtype = ContainerSubType::None);

		// void inline
		void setCustomSkills(SkillRegistry skill_set)	{ custom_skills = skill_set; }
		void setCustomStats(StatRegistry&& stat_set)	{ c_stats = std::make_unique<StatRegistry>(stat_set); }
		void setDestPos(const Position& pos)			{ teleportDestPos = pos; }
		void setDoorId(uint32_t doorId)					{ setIntAttr(ITEM_ATTRIBUTE_DOORID, doorId); }

		void setStrAttr(itemAttrTypes type, std::string_view value)	{ getAttributes()->setStrAttr(type, value); }
		void setIntAttr(itemAttrTypes type, int64_t value)			{ getAttributes()->setIntAttr(type, value); }
		void increaseIntAttr(itemAttrTypes type, int64_t value)		{ getAttributes()->increaseIntAttr(type, value); }

		void removeAttribute(itemAttrTypes type) const
		{
			if (attributes)
			{
				attributes->removeAttribute(type);
			}
		}

		template<typename R>
		void setCustomAttribute(std::string_view key, R value) { getAttributes()->setCustomAttribute(key, value); }
		void setCustomAttribute(std::string_view key, ItemAttributes::CustomAttribute& value) { getAttributes()->setCustomAttribute(key, value); }

		void setSpecialDescription(std::string_view desc) { setStrAttr(ITEM_ATTRIBUTE_DESCRIPTION, desc); }
		void setText(std::string_view text)		{ setStrAttr(ITEM_ATTRIBUTE_TEXT, text); }
		void resetText() const					{ removeAttribute(ITEM_ATTRIBUTE_TEXT); }
		void setDate(int32_t n)					{ setIntAttr(ITEM_ATTRIBUTE_DATE, n); }
		void resetDate() const					{ removeAttribute(ITEM_ATTRIBUTE_DATE); }
		void setWriter(std::string_view writer)	{ setStrAttr(ITEM_ATTRIBUTE_WRITER, writer); }
		void resetWriter() const				{ removeAttribute(ITEM_ATTRIBUTE_WRITER); }

		// move to cpp
		void setActionId(uint16_t n)
		{
			if (n < 100)
			{
				n = 100;
			}

			setIntAttr(ITEM_ATTRIBUTE_ACTIONID, n);
		}

		void setCharges(uint16_t n)						{ setIntAttr(ITEM_ATTRIBUTE_CHARGES, n); }
		void setFluidType(uint16_t n)					{ setIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE, n); }
		void setOwner(uint32_t owner)					{ setIntAttr(ITEM_ATTRIBUTE_OWNER, owner); }
		void setCorpseOwner(uint32_t corpseOwner)		{ setIntAttr(ITEM_ATTRIBUTE_CORPSEOWNER, corpseOwner); }
		void setDuration(int32_t time)					{ setIntAttr(ITEM_ATTRIBUTE_DURATION, time); }
		void decreaseDuration(int32_t time)				{ increaseIntAttr(ITEM_ATTRIBUTE_DURATION, -time); }
		void setDecaying(ItemDecayState_t decayState)	{ setIntAttr(ITEM_ATTRIBUTE_DECAYSTATE, decayState); }
		void setDecayTo(int32_t decayTo)				{ setIntAttr(ITEM_ATTRIBUTE_DECAYTO, decayTo); }

		void setStoreItem(bool storeItem)	{ setIntAttr(ITEM_ATTRIBUTE_STOREITEM, static_cast<int64_t>(storeItem)); }
		void setItemCount(uint8_t n)		{ count = n; }

		// move to cpp
		void setDefaultDuration()
		{
			uint32_t duration = getDefaultDuration();
			if (duration != 0)
			{
				setDuration(duration);
			}
		}

		void setLoadedFromMap(bool value) { loadedFromMap = value; }

		void clearParent()
		{
			tileParent.reset();
			inventoryOwner.reset();
			containerParent.reset();
			scriptDetached = false;
		};

		void setTileParent(const TileWeakPtr& tile)
		{
			tileParent = tile;
			inventoryOwner.reset();
			containerParent.reset();
			scriptDetached = false;
		}

		void setInventoryOwner(const PlayerWeakPtr& player)
		{
			inventoryOwner = player;
			tileParent.reset();
			containerParent.reset();
			scriptDetached = false;
		}

		void setScriptDetached()
		{
			scriptDetached = true;
			tileParent.reset();
			inventoryOwner.reset();
			containerParent.reset();
		}

		void setContainerParent(const ItemPtr& containerOwner)
		{
			containerParent = containerOwner;
			tileParent.reset();
			inventoryOwner.reset();
			scriptDetached = false;
		}

	// Any variable's bigger than a pointer should get their own class
	// and that class should have a unique pointer here, lazy initialized
	// Item attributes is full enough and needs to be reworked to be allowed to be bigger

	// Access doesn't affect memory layout,
	// but ordering does, hence the many private/protected access specifiers
	private:
		template <ItemSubType Sub>
		ItemPtr getAsSubType()
		{
			return item_subtype == Sub ? static_shared_this<Item>() : nullptr;
		}

		template <ItemSubType Sub>
		ItemConstPtr getAsSubType() const
		{
			return item_subtype == Sub ? static_shared_this<const Item>() : nullptr;
		}

        SkillRegistry custom_skills{};
		std::unique_ptr<StatRegistry> c_stats;
	protected:
		TileWeakPtr tileParent;
		PlayerWeakPtr inventoryOwner;
		ItemWeakPtr containerParent;
		bool scriptDetached = false;
		std::unique_ptr<ItemContainer> container;
		ContainerPtr containerAlias;

	private:
        std::unique_ptr<ItemAttributes> attributes;
		std::unique_ptr<std::vector<std::shared_ptr<BlackTek::Augment>>> augments;
		uint32_t attack_modifier_count = 0;
		uint32_t defense_modifier_count = 0;
		uint32_t conversion_modifier_count = 0;
		uint32_t reform_modifier_count = 0;
		uint32_t named_modifiers_count = 0;
		uint32_t damage_modifiers_count = 0;
		uint32_t origin_modifiers_count = 0;
		uint32_t creature_modifiers_count = 0;
		uint32_t race_modifiers_count = 0;

		TilePtr resolveDestinationTile();
		Attr_ReadValue readTeleportAttr(AttrTypes_t attr, PropStream& propStream);
		void serializeTeleportAttr(PropWriteStream& propWriteStream) const;
		Position teleportDestPos{};

		Attr_ReadValue readDoorAttr(AttrTypes_t attr, PropStream& propStream);
		Attr_ReadValue readBedAttr(AttrTypes_t attr, PropStream& propStream);
		bool canDoorBeUsed(const PlayerConstPtr& player) const;
		bool canBedBeUsed(const PlayerConstPtr& player) const;
		bool canBedBeRemoved() const { return house == nullptr; }
		void onDoorRemoved();
		void serializeBedAttr(PropWriteStream& propWriteStream) const;
		void updateAppearance(const PlayerConstPtr& player);
		void regeneratePlayer(const PlayerPtr& player) const;
		void internalSetSleeper(const PlayerConstPtr& player);
		void internalRemoveSleeper();
		uint64_t sleepStart = 0;
		uint32_t sleeperGUID = 0;

		House* house = nullptr;
		std::unique_ptr<AccessList> accessList;

		static bool canSend(const ItemConstPtr& item);
		bool getReceiver(const ItemPtr& item, std::string& name) const;
		bool sendItem(const ItemPtr& item) const;

		int64_t magicFieldCreateTime = 0;

	protected:
		uint16_t id; // the same id as in ItemType
		ItemSubType item_subtype = ItemSubType::None;

	private:
		uint8_t count = 1; // number of stacked items
		bool loadedFromMap = false;
        std::string getWeightDescription(uint32_t weight) const;
};

using ItemList = std::list<ItemPtr>;
using ItemDeque = std::deque<ItemPtr>;

#endif
