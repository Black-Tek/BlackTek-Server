# Contribution Guidelines
***The purpose of this documentation is to give the contributor a clear understanding on what standards are expected for code contributions on the BlackTek Server project.  The contributor should refer to this document first whenever trying to determine if their code complies with our standards. Shall the contributor not find a sufficient answer to their query here, they shall refer to other code examples within the project if they wish, but  be aware we reserve the right to deny your contribution until our requested change is met. It may be prudent to just ask in our community on discord if you are unsure, as you will most often recieve a clear authoritative answer there quickly.***

## Code Quality
### Memory Management
**When dealing with memory, manual memory management is not allowed! RAII is Life!**

**Instead you must do one of the following :**
>  - **Wrap the object with a smart pointer or other RAII type wrapper and pass that**
>  - **Keep the object inside a container which adheres to RAII, and pass a handle instead**

### Views > Continue

**Prefer filtering a range with a view (`std::views::filter`, or another range adaptor) over using a `continue` statement to skip iterations inside a loop body.**

**Why :**
>  - **A `continue` driven loop mixes the filtering condition with the actual work in the same block, so the reader has to mentally separate "what gets skipped" from "what gets done".**
>  - **A view expresses the filter once, up front, so the loop body only ever contains the real work.**

```C++
// avoid: continue used to skip iterations
for (const auto& modifier : modifiers)
{
    if (not applied(modifier))
        continue;

    accumulate(modifier);
}

// prefer: filter the range with a view first
auto filtered = modifiers | std::views::filter(applied);
for (const auto& modifier : filtered)
    accumulate(modifier);
```

### Namespaces

**New code should nest under the project's root namespace by system category, `BlackTek::SystemCategory`, e.g. `BlackTek::World` (map/chunk system), `BlackTek::Net` (networking), `BlackTek::Metrics`, `BlackTek::SIMD`, `BlackTek::Console`, `BlackTek::Containers`, `BlackTek::Store`. This is the convention going forward for any new subsystem.**

**You will also see a number of top-level namespaces that are not nested under `BlackTek`: `Zones`, `Components`, `ObjectPools`, `OTB`, `IOGuild`, `Titan`, `StandardHitChance`, `xtea`. These predate the `BlackTek::SystemCategory` convention, or were kept separate on purpose. They are acknowledged, accepted exceptions, not a pattern to copy for new code, and not something to "clean up" as a drive-by change in an unrelated PR.**

**Rules for namespacing :**
>  - **New subsystems always nest under `BlackTek::SystemCategory`.**
>  - **Do not add new top-level (non-`BlackTek`) namespaces.**
>  - **Do not rename or re-nest an existing top-level namespace as part of an unrelated change.**

```C++
// new subsystem: always nests under BlackTek
namespace BlackTek::World
{
    class Navigator;
}

```

### Aliases > Verbose Typing

**Prefer a named type alias (`using`) over spelling out a verbose or deeply nested type at every call site.**

**Why :**
>  - **An alias gives a stable, readable name to a type that would otherwise be repeated, and often nested, template noise (`std::shared_ptr<Item>`, `std::unordered_map<uint32_t, std::vector<Item>>`, etc.) at every point of use.**
>  - **It centralizes the underlying type in one place, so changing the implementation (e.g. swapping the smart pointer type) only requires updating the alias, not every call site.**

```C++
// avoid: verbose typing repeated everywhere
std::shared_ptr<Item> getWeapon(slots_t slot, bool ignoreAmmo) const;
void addItem(std::shared_ptr<Item> item);

// prefer: alias declared once, used everywhere
using ItemPtr = std::shared_ptr<Item>;

ItemPtr getWeapon(slots_t slot, bool ignoreAmmo) const;
void addItem(ItemPtr item);
```

### Enums

**Nest an enum inside the class that uses it instead of declaring it at namespace scope. A nested enum's name only needs to stay unambiguous within its own class, not the whole codebase, so it can stay short instead of needing a prefix.**

```C++
// avoid: free-floating enum needs a verbose name to stay unambiguous project-wide
enum class CombatBindKey : uint8_t
{
    Level, MagicLevel, SkillLevel, Attack, Defense
};

// prefer: nested inside the class that owns it, reads naturally as Combat::BindKey
class Combat
{
public:
    enum class BindKey : uint8_t
    {
        Level, MagicLevel, SkillLevel, Attack, Defense
    };
};
```

**See `src/combat.h` for this in practice, most of `Combat`'s enums (`Origin`, `DamageType`, `BlockType`, `Situation`, `TargetCode`, `Config`, `BindSource`, `BindKey`) are nested inside the `Combat` class itself.**

**`enum` vs `enum class` :**

**Default to `enum class`. It is scoped, its enumerators don't leak into the surrounding namespace or class, and it can't silently convert to or compare against an unrelated integer.**

**Drop to a plain, unscoped `enum` only when the enum's raw numeric value is threaded through the type as an everyday part of its use, not just unwrapped at one or two centralized boundary sites :**
>  - **It's stored directly in a plain integer field rather than a field of the enum's own type, e.g. `Combat`'s `uint8_t origin = Origin::None;`.**
>  - **It's used, without a cast, as a `std::bitset`/array index at many call sites throughout the class, e.g. `Combat`'s `config.set(Config::TrueDamage)`, repeated for every flag in `Config`.**
>  - **It needs to interoperate directly with an existing plain-integer system (Lua bindings, network opcode serialization, the legacy `ReturnValue` enum), where a cast at every one of many call sites would add more noise than the type safety is worth.**

**Holding bit-flag values isn't, on its own, a reason to drop to a plain enum. If it's only ever unwrapped to a raw integer at a small number of centralized boundary sites (parsing a config string, writing into a storage field), keep it as `enum class` and cast at just those sites. See `Zones::ZoneFlag`, whose values are only unwrapped via `static_cast<uint32_t>` where they're written into a plain `uint32_t` flags field.**

```C++
// plain enum: the raw value IS the storage type, used directly and pervasively
enum Origin : uint8_t
{
    None, Condition, Spell, Melee, Ranged, /* ... */
};

uint8_t origin = Origin::None; // no cast needed to store it

// enum class: a closed set of named values, kept scoped and type-safe
enum class BindKey : uint8_t
{
    Level, MagicLevel, SkillLevel, /* ... */
};

BindKey key = BindKey::Level; // stored as its own type, not a raw integer

// enum class flags: bitwise values, but still scoped, cast only at the
// few boundary sites that actually need the raw bits (parsing/storage)
enum class ZoneFlag : uint32_t
{
    None = 0,
    Protection = 1 << 0,
    NoPvp = 1 << 1,
};

uint32_t flags = 0;
flags |= static_cast<uint32_t>(ZoneFlag::Protection);
```

### noexcept

**Mark a function `noexcept` unless it can genuinely throw. This is the default, not the exception, it applies to getters, setters, and private helpers just as much as public API.**

**Why :**
>  - **`noexcept` documents the function's contract for the reader, and lets the compiler skip the stack-unwinding machinery it would otherwise have to generate for it.**
>  - **A `noexcept` call chain composes safely, callers don't need to trace whether some function three calls deep might throw.**

```C++
// prefer: noexcept unless the function can genuinely throw
[[nodiscard]] bool isTrueDamage() const noexcept { return GetConfig(Config::TrueDamage); }
void SetDamageType(uint16_t type) noexcept { damage_type = type; }
[[nodiscard]] static bool IsAreaTileClear(uint16_t x, uint16_t y, uint8_t z) noexcept;
```

### [[nodiscard]]

**Mark a function `[[nodiscard]]` whenever its return value has no side effect and silently discarding it would be a mistake, getters, predicates (`is*`/`has*`/`Get*`), and pure computations.**

```C++
[[nodiscard]] TargetCode target(const PlayerPtr& attacker, const PlayerPtr& victim) const noexcept;
[[nodiscard]] bool GetConfig(Config flag) const noexcept { return config.test(flag); }
[[nodiscard]] static int32_t calculate_output(const OutputFactors& factors, int32_t stat) noexcept;
```

**A function that mutates state or has a meaningful side effect beyond producing its return value, `SetConfig`, `execute`, does not need `[[nodiscard]]`, a caller may legitimately ignore its result.**

### Struct vs Class

**Use `struct` for a plain aggregate, a bag of public data with no invariant to protect. Use `class` once a type has private state, or a constructor whose job is to establish and protect an invariant.**

```C++
// struct: plain aggregate, all public, no invariant to protect
struct LeechData
{
    int32_t percent_health = 0;
    int32_t percent_mana = 0;
};

struct FormulaContext
{
    CreaturePtr caster;
    CreaturePtr target;
};

// class: private state, the constructor establishes an invariant
class AreaCombat
{
public:
    void setupArea(int32_t radius);
    [[nodiscard]] CombatArea GetCombatArea() const;

private:
    std::vector<MatrixArea> areas;
    bool hasExtendedArea = false;
};
```

**See `src/combat.h` : `LeechData`, `DamageArea`, `OutputFactors`, `FormulaContext`, and `CompiledFormulaSlots` are all `struct`; `AreaCombat`, `Combat`, and `CombatRegistry`, each of which owns private state and enforces an invariant through its constructor, are all `class`.**

### Logging

**Never use `std::cout`, `std::cerr`, `printf`, or a hand-rolled logging helper. Use `BlackTek::Console` for every print, log, or debug message. It already handles async, non-blocking writes, per-channel file rotation, flood suppression, and colored console output, none of which a one-off `std::cout` call gets for free.**

**Which function to call :**
>  - **`Console::Info` / `Warn` / `Error` / `Fatal` : the general "System" channel. Use these for anything that doesn't belong to a more specific subsystem.**
>  - **The subsystem namespaces, `Console::Net`, `Console::Database`, `Console::Script`, `Console::Combat`, `Console::Map`, `Console::Player`, `Console::Admin`, `Console::Security`, mirror the same `Info`/`Warn`/`Error`/`Fatal` set, scoped to that subsystem's own log channel and file. Use whichever one matches the code you're writing rather than always reaching for the System channel.**
>  - **`Console::Debug` / `Console::Trace` : same idea, but automatically stamp the call site's file and line (via `std::source_location`) into the message. Use these for anything that should stay silent in normal operation and only show up once that channel's debug/trace level is enabled.**
>  - **`Console::Print` : an unformatted, unlogged console line, for output that doesn't belong in a log file at all, e.g. startup banners.**

```C++
// avoid
std::cout << "Shutting down..." << std::flush;
std::cerr << "Failed to save account - level storage values." << std::endl;

// prefer
BlackTek::Console::Print("Shutting down...");
BlackTek::Console::Error("Failed to save account - level storage values.");

// prefer the channel scoped variant when the message belongs to a subsystem
BlackTek::Console::Database::Error("Failed to initialize MySQL connection handle.");
```

**Message text follows a `ClassName::methodName: message` prefix, so a log line can be traced back to its call site even without `Console::Debug`'s automatic source location :**

```C++
BlackTek::Console::Error("Combat::apply_effects: Item::CreateItem returned null for item id {}", itemId);
BlackTek::Console::Warn("AccountManager::initialize: character option '{}' has no 'outfit' table", index.str());
```

**See `src/console.h` for the full API. Its own comment above the `Info`/`Warn`/`Error`/`Debug`/`Trace`/`Fatal` block says it plainly: "These are the intended 'Debugger' methods to be used in the codebase."**

**You will still find `std::cout`/`std::cerr` scattered through older files. Those predate `BlackTek::Console` and are not a pattern to copy. Migrate them opportunistically if you're already touching that code, but don't go out of your way to change unrelated lines in an unrelated PR.**

## Code Style

### Braces { }

**Unbraced Statements are not required, but highly prefered and should only be used for the following situations :**
>  - **Return only statements**
>  - **Single statements for a conditional which exists as the most inner of it's own scope.**
>  - **Preferentially, when a chain of nested `if`/`else if`/`else` blocks at the most inner scope goes unbraced, put a blank line before each `if`, `else if`, and `else` to visually separate the branches from one another.**

```C++
// return only statement, and it's the innermost scope: unbraced is fine
if (not creature)
    return false;

// a nested if/else if/else chain, unbraced because it's the innermost scope,
// with a blank line preceding each branch
if (skill == SKILL_CLUB)
    return "Club Fighting";

else if (skill == SKILL_SWORD)
    return "Sword Fighting";

else if (skill == SKILL_AXE)
    return "Axe Fighting";

else
    return "Fist Fighting";
```

**Rules for using Braces :**
>  - **A new scope must begin on a new line with that line containing only the opening brace**
>  - **Each closing of scope must be on it's own line, containing only the closing brace.**

 **Exception: when an inline function definition is short enough it can remain on same line,
  example:**

```C++
const int getNumber() const { return some_value; } 
```

### Alignment

**When a block of code repeats the same shape, consecutive member variables, consecutive function declarations, `switch`/`case` bodies, struct fields, it should be visually aligned into columns using tabs, not spaces. This is purely aesthetic, it must never change semantics.**

**Rules for aligning code :**
>  - **Align on a shared "name column" : find the longest type prefix (return type plus any leading `const`/`static`/`[[nodiscard]]` qualifiers) among the group, round that length up to the next multiple of 4, and pad every member's type prefix with tabs to reach it.**
>  - **For inline members whose body sits on the same line, also align a "brace column" : the longest `name column + signature length` in the group, rounded up to the next multiple of 4, padded with tabs before the opening `{`.**
>  - **Alignment groups are local, not global. Only align members that already sit together as a natural cluster of the same kind, e.g. every inline getter returning `ItemPtr`, or every `case` label in one `switch`. Don't stretch one column across an entire class or file.**
>  - **Preserve the existing relative order of members within a group. Alignment is a whitespace pass, not a re-sort.**
>  - **One or two dramatic outliers in an otherwise tight cluster should be excluded from the column width calculation, format them against the resulting column and let them overflow with a single space rather than dragging the whole column out wider for everyone else.**

```C++
// before: name column not aligned
ItemPtr getWeapon(slots_t slot, bool ignoreAmmo) const;
ContainerPtr getContainerByID(uint8_t cid);
ContainerPtr getDepotChest(uint32_t depotId, bool autoCreate);

// after: longest type prefix ("ContainerPtr", 12 chars) rounds up to 12 -> 16 columns,
// every prefix padded with tabs to reach it
ItemPtr			getWeapon(slots_t slot, bool ignoreAmmo) const;
ContainerPtr	getContainerByID(uint8_t cid);
ContainerPtr	getDepotChest(uint32_t depotId, bool autoCreate);
```

```C++
// inline members also get a brace column, padded from (name column + signature length)
// before
float getAttackFactor() const override { return attackFactor; }
float getDefenseFactor() const override { return defenseFactor; }

// after
float	getAttackFactor() const override	{ return attackFactor; }
float	getDefenseFactor() const override	{ return defenseFactor; }
```

```C++
// outlier example: ten short pointer aliases share a column, the one long
// outlier is excluded from the width calculation and simply overflows with a single space
ItemPtr			getWeapon(slots_t slot, bool ignoreAmmo) const;
ItemPtr			getWriteItem(uint32_t& windowTextId, uint16_t& maxWriteLen);
std::unique_ptr<BlackTek::AreaCombat> getSpellArea();
```

**When not to align :**
>  - **If every entry in a group shares the exact same type text (a run of plain `bool` or plain `void` with no qualifier variation), leave it alone. A single space already lines them up, and forcing a tab in only adds a needless jump.**
>  - **If a group's lengths are spread out continuously with no natural cluster to align on, for example a long block of `send*`/`on*` methods ranging from ~20 to ~150 characters, do not force one shared column. That produces either absurd padding or a mostly-overflowing block. Leave that whitespace as-is.**

```C++
// skip alignment: every entry is plain "bool", a single space already lines them up
bool isPremium() const;
bool isPushable() const;
bool canSee(const Position& pos) const override;
```

**Tab width is 4 columns, matching this project's existing indentation. See `src/player.h` for the reference implementation, most notably the `getPositionForSlot` switch statement, the `BlackTek::ModifierCache` struct, and the aligned function declaration blocks throughout the `Player` class.**

### Casing

**Rules for casing :**
>  - **Object member fields, non-static data members belonging to a class or struct instance, use `snake_case`.**
>  - **Local variables, anything declared inside the body of a function or method, use `camelCase`.**
>  - **Parameters follow the same rule as local variables, they use `camelCase`.**
>  - **Everything named at global or namespace scope, global variables, static members, aliases (`using`/`typedef`), namespaces, and functions, uses `PascalCase`.**

```C++
namespace BlackTek // namespace: PascalCase
{
	using ItemList = std::vector<ItemPtr>; // alias: PascalCase

	static uint32_t TotalItemsLoaded = 0; // static member / global-scope storage: PascalCase

	class ItemManager // class: PascalCase
	{
		public:
			void LoadItems(uint32_t startingId); // function: PascalCase, parameter: camelCase

		private:
			uint32_t item_count; // object member field: snake_case
			std::string default_name; // object member field: snake_case
	};

	void ItemManager::LoadItems(uint32_t startingId)
	{
		uint32_t itemsLoaded = 0; // local variable: camelCase

		for (uint32_t currentId = startingId; currentId < item_count; ++currentId) // currentId: camelCase
		{
			++itemsLoaded;
		}
	}
}
```

### Keywords and Operators

**Do not use the logical operator :**

> - **``&&`` for evaluation. Use the keyword ``and`` in it's place.**
> - **``!`` for evalutaion. Use the keyword ``not`` in it's place.**
> - **``||`` for evaluation. Use the keyword ``or`` in it's place.**

```C++
// correct
if (not file)
{
    return;
}

if (condA and condB)
{
    ...
}

if (condA or condB)
{
    ...
}

// incorrect
if (!file) { ... }
if (condA && condB) { ... }
if (condA || condB) { ... }
```

### Casts

**Always use a named cast, `static_cast`, `std::to_underlying`, `reinterpret_cast`, `const_cast`, never a C-style cast.**

**Why :**
>  - **A named cast says what kind of conversion is happening and is searchable. A C-style cast can silently perform a `static_cast`, `const_cast`, or `reinterpret_cast` depending on the types involved, and the reader can't tell which without checking each one.**

```C++
// avoid
const uint64_t added = (uint64_t)currentDamage * percent_weak / 100;

// prefer
const uint64_t added = static_cast<uint64_t>(currentDamage) * percent_weak / 100;
```

### Aggregate Initialization for Presets

**When declaring a `static constexpr` aggregate constant that represents one named preset out of a family of presets, initialize it with C++20 designated initializers (`.field = value`) rather than positional aggregate initialization, so each preset only has to spell out the fields it actually overrides.**

```C++
// avoid: positional init, every field position must be known, and unnamed fields are silently defaulted
static constexpr ResistanceFactors ClassicArmor{ 0.5f, 1.0f, 0.0f, 0.0f, 3.0f, 1.0f, 100.0f, 1, ResistanceFormula::Parity };

// prefer: designated initializers, only the overridden fields are named
static constexpr ResistanceFactors ClassicArmor
{
    .min_scale     = 0.5f,
    .threshold     = 3.0f,
    .flat_amount   = 1.0f,
    .parity_offset = 1,
    .formula_type  = ResistanceFormula::Parity
};
```

**See `Combat::ClassicOutput`, `Combat::ScaledResolution`, `Combat::BalancedResolution`, and the dozen other presets in `src/combat.h`.**

### Includes

**Order includes as: this project's own headers first (quoted, `"header.h"`), then a blank line, then standard library and third-party headers (angle brackets, `<vector>`).**

```C++
#include "metrics_types.h"
#include "item.h"
#include "condition.h"
#include "creature.h"

#include <utility>
#include <optional>
#include <span>
#include <gtl/phmap.hpp>
```

**See `src/combat.h` and `src/player.h`, both follow this ordering.**