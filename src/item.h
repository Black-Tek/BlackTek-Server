// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_ITEM_H
#define FS_ITEM_H

#include "cylinder.h"
#include "thing.h"
#include "items.h"
#include "luascript.h"
#include "tools.h"
#include "imbuement.h"
#include "augments.h"
#include "declarations.h"

#include <typeinfo>
#include <boost/variant.hpp>
#include <deque>
#include <gtl/phmap.hpp>

class Creature;
class Player;
class Container;
class Depot;
class Teleport;
class TrashHolder;
class Mailbox;
class Door;
class MagicField;
class BedItem;


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
	ATTR_IMBUESLOTS = 43,
	ATTR_IMBUEMENTS = 44,
	ATTR_REWARDID = 45
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
			| ITEM_ATTRIBUTE_ATTACK_SPEED | ITEM_ATTRIBUTE_REWARDID;
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

class Item : virtual public Thing, public SharedObject
{
	public:
		//Factory member to create item of right type based on type
		static ItemPtr CreateItem(const uint16_t type, uint16_t count = 0);
		static ContainerPtr CreateItemAsContainer(const uint16_t type, uint16_t size);
		bool giveCustomSkill(std::string_view name, uint16_t level);
		bool giveCustomSkill(std::string_view name, std::shared_ptr<CustomSkill> new_skill);
		bool removeCustomSkill(std::string_view name);
		std::shared_ptr<CustomSkill> getCustomSkill(std::string_view name);

		const gtl::flat_hash_map<std::string, std::shared_ptr<CustomSkill>>& getCustomSkills() const
		{
			return custom_skills;
		}

		void setCustomSkills(SkillRegistry skill_set)
		{
			custom_skills = skill_set;
		}
		static ItemPtr CreateItem(PropStream& propStream);
		static Items items;

		// Constructor for items
		explicit Item(const uint16_t type, uint16_t count = 0);
		Item(const Item& i);
		virtual ItemPtr clone() const;

		~Item() = default;

		// non-assignable
		Item& operator=(const Item&) = delete;

		bool equals(const ItemConstPtr& otherItem) const;
	
		ItemPtr getItem() override final {
			return static_shared_this<Item>();
		}

		ItemConstPtr getItem() const override final {
			return static_shared_this<const Item>();
		}
	
		virtual TeleportPtr getTeleport() {
			return nullptr;
		}
	
		virtual TeleportConstPtr getTeleport() const {
			return nullptr;
		}
	
		virtual TrashHolderPtr getTrashHolder() {
			return nullptr;
		}
	
		virtual TrashHolderConstPtr getTrashHolder() const {
			return nullptr;
		}
	
		virtual MailboxPtr getMailbox() {
			return nullptr;
		}
	
		virtual MailboxConstPtr getMailbox() const {
			return nullptr;
		}
	
		virtual DoorPtr getDoor() {
			return nullptr;
		}
	
		virtual DoorConstPtr getDoor() const {
			return nullptr;
		}
	
		virtual MagicFieldPtr getMagicField() {
			return nullptr;
		}
	
		virtual MagicFieldConstPtr getMagicField() const {
			return nullptr;
		}
	
		virtual BedItemPtr getBed() {
			return nullptr;
		}
	
		virtual BedItemConstPtr getBed() const {
			return nullptr;
		}

		const std::string& getStrAttr(itemAttrTypes type) const {
			if (!attributes) {
				return ItemAttributes::emptyString;
			}
			return attributes->getStrAttr(type);
		}

		void setStrAttr(itemAttrTypes type, std::string_view value) 
		{
			getAttributes()->setStrAttr(type, value); 
		}

		int64_t getIntAttr(itemAttrTypes type) const {
			if (!attributes) {
				return 0;
			}
			return attributes->getIntAttr(type);
		}
	
		void setIntAttr(itemAttrTypes type, int64_t value) {
			getAttributes()->setIntAttr(type, value);
		}
	
		void increaseIntAttr(itemAttrTypes type, int64_t value) {
			getAttributes()->increaseIntAttr(type, value);
		}

		void removeAttribute(itemAttrTypes type) const {
			if (attributes) {
				attributes->removeAttribute(type);
			}
		}
	
		bool hasAttribute(itemAttrTypes type) const {
			if (!attributes) {
				return false;
			}
			return attributes->hasAttribute(type);
		}

		template<typename R>
		void setCustomAttribute(std::string_view key, R value) {
			getAttributes()->setCustomAttribute(key, value);
		}

		void setCustomAttribute(std::string_view key, ItemAttributes::CustomAttribute& value) {
			getAttributes()->setCustomAttribute(key, value);
		}

		const ItemAttributes::CustomAttribute* getCustomAttribute(int64_t key) {
			if (!attributes) {
				return nullptr;
			}
			return getAttributes()->getCustomAttribute(key);
		}

		const ItemAttributes::CustomAttribute* getCustomAttribute(const std::string& key) {
			if (!attributes) {
				return nullptr;
			}
			return getAttributes()->getCustomAttribute(key);
		}

		bool removeCustomAttribute(int64_t key) {
			if (!attributes) {
				return false;
			}
			return getAttributes()->removeCustomAttribute(key);
		}

		bool removeCustomAttribute(std::string_view key) {
			if (!attributes) {
				return false;
			}
			return getAttributes()->removeCustomAttribute(key);
		}

		void setSpecialDescription(std::string_view desc) { 
			setStrAttr(ITEM_ATTRIBUTE_DESCRIPTION, desc); 
		}

		const std::string& getSpecialDescription() const {
			return getStrAttr(ITEM_ATTRIBUTE_DESCRIPTION);
		}

		void setText(std::string_view text) { 
			setStrAttr(ITEM_ATTRIBUTE_TEXT, text); 
		}

		void resetText() const {
			removeAttribute(ITEM_ATTRIBUTE_TEXT);
		}
	
		const std::string& getText() const {
			return getStrAttr(ITEM_ATTRIBUTE_TEXT);
		}

		void setDate(int32_t n) {
			setIntAttr(ITEM_ATTRIBUTE_DATE, n);
		}
	
		void resetDate() const
		{
			removeAttribute(ITEM_ATTRIBUTE_DATE);
		}
	
		time_t getDate() const {
			return static_cast<time_t>(getIntAttr(ITEM_ATTRIBUTE_DATE));
		}

		void setWriter(std::string_view writer) { 
			setStrAttr(ITEM_ATTRIBUTE_WRITER, writer); 
		}

		void resetWriter() const {
			removeAttribute(ITEM_ATTRIBUTE_WRITER);
		}
	
		const std::string& getWriter() const {
			return getStrAttr(ITEM_ATTRIBUTE_WRITER);
		}

		void setActionId(uint16_t n) {
			if (n < 100) {
				n = 100;
			}

			setIntAttr(ITEM_ATTRIBUTE_ACTIONID, n);
		}
	
		uint16_t getActionId() const {
			if (!attributes) {
				return 0;
			}
			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_ACTIONID));
		}

		uint16_t getUniqueId() const {
			if (!attributes) {
				return 0;
			}
			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_UNIQUEID));
		}

		void setCharges(uint16_t n) {
			setIntAttr(ITEM_ATTRIBUTE_CHARGES, n);
		}
	
		uint16_t getCharges() const {
			if (!attributes) {
				return 0;
			}
			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_CHARGES));
		}

		void setFluidType(uint16_t n) {
			setIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE, n);
		}
	
		uint16_t getFluidType() const {
			if (!attributes) {
				return 0;
			}
			return static_cast<uint16_t>(getIntAttr(ITEM_ATTRIBUTE_FLUIDTYPE));
		}

		void setOwner(uint32_t owner) {
			setIntAttr(ITEM_ATTRIBUTE_OWNER, owner);
		}
	
		uint32_t getOwner() const {
			if (!attributes) {
				return 0;
			}
			return getIntAttr(ITEM_ATTRIBUTE_OWNER);
		}

		void setCorpseOwner(uint32_t corpseOwner) {
			setIntAttr(ITEM_ATTRIBUTE_CORPSEOWNER, corpseOwner);
		}
	
		uint32_t getCorpseOwner() const {
			if (!attributes) {
				return 0;
			}
			return getIntAttr(ITEM_ATTRIBUTE_CORPSEOWNER);
		}

		void setDuration(int32_t time) {
			setIntAttr(ITEM_ATTRIBUTE_DURATION, time);
		}
	
		void decreaseDuration(int32_t time) {
			increaseIntAttr(ITEM_ATTRIBUTE_DURATION, -time);
		}
	
		uint32_t getDuration() const {
			if (!attributes) {
				return 0;
			}
			return getIntAttr(ITEM_ATTRIBUTE_DURATION);
		}

		void setDecaying(ItemDecayState_t decayState) {
			setIntAttr(ITEM_ATTRIBUTE_DECAYSTATE, decayState);
		}
	
		ItemDecayState_t getDecaying() const {
			if (!attributes) {
				return DECAYING_FALSE;
			}
			return static_cast<ItemDecayState_t>(getIntAttr(ITEM_ATTRIBUTE_DECAYSTATE));
		}

		int32_t getDecayTime() const {
			if (hasAttribute(ITEM_ATTRIBUTE_DURATION)) {
				return getIntAttr(ITEM_ATTRIBUTE_DURATION);
			}
			return items[id].decayTime;
		}

		void setDecayTo(int32_t decayTo) {
			setIntAttr(ITEM_ATTRIBUTE_DECAYTO, decayTo);
		}
	
		int32_t getDecayTo() const {
			if (hasAttribute(ITEM_ATTRIBUTE_DECAYTO)) {
				return getIntAttr(ITEM_ATTRIBUTE_DECAYTO);
			}
			return items[id].decayTo;
		}

		const bool isEquipped();
		void decayImbuements(bool infight);

		static std::string getDescription(const ItemType& it, int32_t lookDistance, const ItemConstPtr& item = nullptr, int32_t subType = -1, bool addArticle = true);
		static std::string getNameDescription(const ItemType& it, const ItemConstPtr& item = nullptr, int32_t subType = -1, bool addArticle = true);
		static std::string getWeightDescription(const ItemType& it, uint32_t weight, uint32_t count = 1);

		std::string getDescription(int32_t lookDistance) const override final;
		std::string getNameDescription() const;
		std::string getWeightDescription() const;

		//serialization
		virtual Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream);
		bool unserializeAttr(PropStream& propStream);
		bool unserializeAugments(PropStream& propStream);
		virtual bool unserializeItemNode(OTB::Loader&, const OTB::Node&, PropStream& propStream);

		virtual void serializeAttr(PropWriteStream& propWriteStream) const;

		bool isPushable() const override final {
			return isMoveable();
		}
	
		int32_t getThrowRange() const override final {
			return (isPickupable() ? 15 : 2);
		}

		uint16_t getID() const {
			return id;
		}
	
		uint16_t getClientID() const {
			return items[id].clientId;
		}
	
		void setID(uint16_t newid);

		// Returns the player that is holding this item in his inventory
		PlayerPtr getHoldingPlayer();

		WeaponType_t getWeaponType() const {
			return items[id].weaponType;
		}
	
		Ammo_t getAmmoType() const {
			return items[id].ammoType;
		}
	
		uint8_t getShootRange() const {
			if (hasAttribute(ITEM_ATTRIBUTE_SHOOTRANGE)) {
				return getIntAttr(ITEM_ATTRIBUTE_SHOOTRANGE);
			}
			return items[id].shootRange;
		}

		virtual uint32_t getWeight() const;
	
		uint32_t getBaseWeight() const {
			if (hasAttribute(ITEM_ATTRIBUTE_WEIGHT)) {
				return getIntAttr(ITEM_ATTRIBUTE_WEIGHT);
			}
			return items[id].weight;
		}
	
		int32_t getAttack() const {
			if (hasAttribute(ITEM_ATTRIBUTE_ATTACK)) {
				return getIntAttr(ITEM_ATTRIBUTE_ATTACK);
			}
			return items[id].attack;
		}
	
		uint32_t getAttackSpeed() const {
			if (hasAttribute(ITEM_ATTRIBUTE_ATTACK_SPEED)) {
				return getIntAttr(ITEM_ATTRIBUTE_ATTACK_SPEED);
			}
			return items[id].attackSpeed;
		}
	
		int32_t getArmor() const {
			if (hasAttribute(ITEM_ATTRIBUTE_ARMOR)) {
				return getIntAttr(ITEM_ATTRIBUTE_ARMOR);
			}
			return items[id].armor;
		}
	
		int32_t getDefense() const {
			if (hasAttribute(ITEM_ATTRIBUTE_DEFENSE)) {
				return getIntAttr(ITEM_ATTRIBUTE_DEFENSE);
			}
			return items[id].defense;
		}
	
		int32_t getExtraDefense() const {
			if (hasAttribute(ITEM_ATTRIBUTE_EXTRADEFENSE)) {
				return getIntAttr(ITEM_ATTRIBUTE_EXTRADEFENSE);
			}
			return items[id].extraDefense;
		}
	
		int32_t getSlotPosition() const {
			return items[id].slotPosition;
		}
	
		int8_t getHitChance() const {
			if (hasAttribute(ITEM_ATTRIBUTE_HITCHANCE)) {
				return getIntAttr(ITEM_ATTRIBUTE_HITCHANCE);
			}
			return items[id].hitChance;
		}

		uint16_t getEquipSlot() const {
			return items[id].equipSlot;
		}

		uint32_t getWorth() const;
		LightInfo getLightInfo() const;

		bool hasProperty(ITEMPROPERTY prop) const;
	
		bool isBlocking() const {
			return items[id].blockSolid;
		}
	
		bool isStackable() const {
			return items[id].stackable;
		}
	
		bool isAlwaysOnTop() const {
			return items[id].alwaysOnTop;
		}
	
		bool isGroundTile() const {
			return items[id].isGroundTile();
		}
	
		bool isMagicField() const {
			return items[id].isMagicField();
		}
	
		bool isMoveable() const {
			return items[id].moveable;
		}
	
		bool isPickupable() const {
			return items[id].pickupable;
		}
	
		bool isUseable() const {
			return items[id].useable;
		}
	
		bool isHangable() const {
			return items[id].isHangable;
		}
	
		bool isRotatable() const {
			const ItemType& it = items[id];
			return it.rotatable && it.rotateTo;
		}
	
		bool hasWalkStack() const {
			return items[id].walkStack;
		}

		void setStoreItem(bool storeItem) {
			setIntAttr(ITEM_ATTRIBUTE_STOREITEM, static_cast<int64_t>(storeItem));
		}
	
		bool isStoreItem() const {
			if (hasAttribute(ITEM_ATTRIBUTE_STOREITEM)) {
				return getIntAttr(ITEM_ATTRIBUTE_STOREITEM) == 1;
			}
			return items[id].storeItem;
		}
	
		const std::string& getName() const {
			if (hasAttribute(ITEM_ATTRIBUTE_NAME)) {
				return getStrAttr(ITEM_ATTRIBUTE_NAME);
			}
			return items[id].name;
		}
	
		const std::string getPluralName() const {
			if (hasAttribute(ITEM_ATTRIBUTE_PLURALNAME)) {
				return getStrAttr(ITEM_ATTRIBUTE_PLURALNAME);
			}
			return items[id].getPluralName();
		}
	
		const std::string& getArticle() const {
			if (hasAttribute(ITEM_ATTRIBUTE_ARTICLE)) {
				return getStrAttr(ITEM_ATTRIBUTE_ARTICLE);
			}
			return items[id].article;
		}
	
		const std::string getClassification() const {
			if (hasAttribute(ITEM_ATTRIBUTE_CLASSIFICATION)) {
				return getStrAttr(ITEM_ATTRIBUTE_CLASSIFICATION);
			}
			return items[id].classification;
		}
	
		const std::string getTier() const {
			if (hasAttribute(ITEM_ATTRIBUTE_TIER)) {
				return getStrAttr(ITEM_ATTRIBUTE_TIER);
			}
			return items[id].tier;
		}

		// get the number of items
		uint16_t getItemCount() const {
			return count;
		}
	
		void setItemCount(uint8_t n) {
			count = n;
		}

		static uint32_t countByType(const ItemConstPtr& i, int32_t subType) {
			if (subType == -1 || subType == i->getSubType()) {
				return i->getItemCount();
			}

			return 0;
		}

		void setDefaultSubtype();
		uint16_t getSubType() const;
		void setSubType(uint16_t n);

		void setUniqueId(uint16_t n);

		void setDefaultDuration() {
			uint32_t duration = getDefaultDuration();
			if (duration != 0) {
				setDuration(duration);
			}
		}
	
		uint32_t getDefaultDuration() const {
			return items[id].decayTime * 1000;
		}
	
		bool canDecay();

		virtual bool canRemove() const {
			return true;
		}
	
		virtual bool canTransform() const {
			return true;
		}
	
		virtual void onRemoved();
		virtual void onTradeEvent(TradeEvents_t, const PlayerPtr&) {}

		virtual void startDecaying();

		bool isLoadedFromMap() const {
			return loadedFromMap;
		}
	
		void setLoadedFromMap(bool value) {
			loadedFromMap = value;
		}
	
		bool isCleanable() const {
			return !loadedFromMap && canRemove() && isPickupable() && !hasAttribute(ITEM_ATTRIBUTE_UNIQUEID) && !hasAttribute(ITEM_ATTRIBUTE_ACTIONID);
		}

		bool hasMarketAttributes() const;

		std::unique_ptr<ItemAttributes>& getAttributes() {
			if (!attributes) {
				attributes.reset(new ItemAttributes());
			}
			return attributes;
		}
	
		CylinderPtr getParent() override {
			auto lockedParent = parent.lock();
			if (!lockedParent) {
				// todo add logger here
			}
			return lockedParent;
		}

		CylinderConstPtr getParent() const override {
			auto lockedParent = parent.lock();
			if (!lockedParent) {
				// todo add logger here
			}
			return lockedParent;
		}

	
		void setParent(std::weak_ptr<Cylinder> cylinder) override {
			parent = cylinder;
		}

		void clearParent() override
		{
			parent.reset();
		};
	
		CylinderPtr getTopParent();
		CylinderConstPtr getTopParent() const;
		TilePtr getTile() override;
		std::shared_ptr<const Tile> getTile() const override;
	
		bool isRemoved() const override {
			auto parentLock = parent.lock();

			if (!parentLock) {
				return false;
			}

			return parentLock->isRemoved();
		}

		uint16_t getImbuementSlots() const;
		uint16_t getFreeImbuementSlots() const;
		bool canImbue();
		bool addImbuementSlots(const uint16_t amount);
		bool removeImbuementSlots(const uint16_t amount, const bool destroyImbues = false);
		bool hasImbuementType(const ImbuementType imbuetype) const;
		bool hasImbuement(const std::shared_ptr<Imbuement>& imbuement) const;
		bool hasImbuements() const; /// change to isImbued();
		bool addImbuement(std::shared_ptr<Imbuement> imbuement, bool created = true);
		bool removeImbuement(const std::shared_ptr<Imbuement>& imbuement, bool decayed = false);
		std::vector<std::shared_ptr<Imbuement>>& getImbuements();
		const std::vector<std::shared_ptr<Imbuement>>& getImbuements() const;


		const bool addAugment(std::string_view augmentName);
		const bool addAugment(const std::shared_ptr<Augment>& augment);
		
		const bool removeAugment(std::string_view name);
		const bool removeAugment(std::shared_ptr<Augment>& augment);

		bool isAugmented() const;
		bool hasAugment(std::string_view name) const;
		bool hasAugment(const std::shared_ptr<Augment>& augment) const;

		const std::vector<std::shared_ptr<Augment>>& getAugments();

	protected:
		std::weak_ptr<Cylinder> parent;

		uint16_t id; // the same id as in ItemType

	private:
		std::string getWeightDescription(uint32_t weight) const;

		std::unique_ptr<ItemAttributes> attributes;

		uint16_t imbuementSlots = 0;
		std::vector<std::shared_ptr<Imbuement>> imbuements{};
		std::vector<std::shared_ptr<Augment>> augments{};

		SkillRegistry custom_skills{};

		uint8_t count = 1; // number of stacked items
		bool loadedFromMap = false;

		//Don't add variables here, use the ItemAttribute class.
};

using ItemList = std::list<ItemPtr>;
using ItemDeque = std::deque<ItemPtr>;

#endif
