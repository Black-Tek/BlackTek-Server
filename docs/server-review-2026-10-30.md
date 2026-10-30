# Server review, 30 October 2026

Base: `master` at `6facaa09e4854f2bc5bc71b4c7bee7a23eabbbd3`.

This review inventories every tracked file, including nested data folders, and
records the level of inspection in [the coverage ledger](server-review-coverage.csv).
All 182 C++ source/header files were read manually. Most content files received
format, reference and registration checks, not a manual line-by-line gameplay
review. Unchanged license texts and binary assets are explicitly distinguished.
Coverage does not establish that a file is free of bugs.

The 1,622-file snapshot contains 327 full manual reviews, 8 partial manual
reviews, 1,274 automated-only checks, 10 license-text inventories and 3 binary
records (one structural check and two inventories). All 40 new implementation
and test files have full manual review records. CSV hashes describe the Windows
working-tree bytes, including local line endings.

No private maps, databases, credentials, binaries or temporary build outputs are
included. Tests use synthetic engine fixtures or deterministic external boundaries.
The repository's existing public sample map is unchanged.

## Fixes

| Area | Correction | Main files |
| --- | --- | --- |
| Packet parsing | Reject truncated fields, invalid skip arithmetic and invalid encrypted/checksum lengths before reads; reject malformed authentication timestamps. | `networkmessage.h`, `protocol.cpp`, `protocolgame.cpp`, `protocollogin.cpp` |
| Connections and login queue | Close sockets when a graceful close escalates, release failed accepts, handle mapped IPv4 safely and correct queue removal/priority accounting. | `connection.cpp`, `server.cpp`, `loginqueue.h` |
| Authentication | Use tested HMAC-SHA1 for token generation, including long keys; enforce the existing token check on the account-manager fallback too. Token-session freshness remains a separate issue below. | `tools.cpp`, `iologindata.cpp` |
| Persistence | Lock transactions before BEGIN, handle failed BEGIN and null/missing query fields safely, propagate custom skill/stat save failures, replace old stat rows and retain offline persistent conditions. | `database.cpp`, `database.h`, `iologindata.cpp` |
| Market | Check offer creation before debiting, claim offers transactionally, check history and credit writes, revalidate stale offers, correct recipient handling and reject missing buyers. | `iomarket.cpp`, `game.cpp` |
| Lua API | Correct argument/stack handling, defaults, type tests, blob lengths, custom skill/stat calls, healing and combat area callbacks; supply the missing spell callback argument. | `luascript.cpp`, `events.cpp`, `spells.cpp` |
| Lua ownership | Keep XML documents alive with their nodes, avoid duplicate ownership in item splitting, preserve transformed userdata and keep pending zones alive before activation callbacks. | `luascript.cpp`, `luascript.h` |
| Items and browse fields | Remove the container self-reference cycle, keep container aliases tied to their item, avoid registry mutation from item destruction and guard bed/offline-player access. | `item.cpp`, `item.h`, `itemcontainer.cpp` |
| Party and store | Keep callback owners alive during reentrancy, validate party XP outputs and split safely on script failure, make unconfigured purchases fail closed, release registry owners before game pools. | `party.cpp`, `party.h`, `storewindow.cpp`, `storewindow.h`, `game.cpp` |
| External requests | Give each request its own CURL handle/header ownership, validate and escape JSON, and bound request time. | `luascript.cpp`, `game.cpp`, `game.h` |
| Map and chunks | Preserve partition insertion metadata and reserved chunks, avoid a tile cache shared across maps, retain incremental-load memory backing, and snapshot contents before removal callbacks. | `chunk.cpp`, `chunkgrid.cpp`, `map.cpp`, `iomap.cpp` |
| Tasks and shutdown | Release rejected/cancelled tasks and suspended coroutine captures; destroy zone/timer frames and Lua userdata before pooled objects' memory resources. | `scheduler.cpp`, `tasks.cpp`, `tasks.h`, `zones.cpp`, `game.cpp`, `game.h`, `otserv.cpp` |
| Zones | Prevent stale-generation access and destruction while executing, snapshot registry iteration around callbacks, validate IDs, check conversion output before marking completion and restore activation on reload. | `zones.cpp`, `zones.h` |
| Combat | Remove the ambient precomputed damage context that leaked between targets and nested converted strikes; preserve sequential strike semantics and SIMD geometry. Fix per-cast healing mutation, null casters and arithmetic in critical, piercing, resistance and leech calculations. | `combat.cpp`, `combat.h`, `simd_kernels.h` |
| Conditions and modifiers | Validate stored lengths/counts, reject ragged combat matrices, correct stat arithmetic and filter modifier names safely. | `condition.cpp`, `augment.cpp`, `damagemodifier.cpp`, `pointbasedstat.h`, `matrixarea.cpp` |
| Players and trade | Widen progress/currency arithmetic, guard invalid container and modifier state, validate trade offers again after callbacks and restore a refused transfer to its owner or inbox. | `player.cpp`, `player.h`, `game.cpp` |
| Decay | Use wide steady-clock timestamps, replace stale equipment timers, expire items in containers and clamp remaining duration. | `game.cpp`, `item.cpp`, `creature.h` |
| Account manager | Validate submitted options, town/vocation/name/password confirmation and database results rather than trusting client choices. | `game.cpp`, `protocolgame.cpp` |
| Content loading | Validate DAT/TOML bounds and preserve definitions on failed DAT reads; retain live monster-type references; make outfit reloads replace data atomically; keep quest IDs unique and use sparse state lookup. | `items.cpp`, `monsters.cpp`, `outfit.cpp`, `quests.cpp` |
| Skills and utilities | Bound representational overflow and nonfinite arithmetic, fix level-removal/progression math and vocation multiplier parsing; handle empty string operations, bounded integer parsing and equality in position ordering. | `skills.cpp`, `skills.h`, `vocation.cpp`, `tools.cpp`, `position.h` |
| Other ownership/width fixes | Remove the NPC script reference cycle, correct monster callback userdata/type, preserve high-bit combat types and wide guild/currency balances, and avoid division by disabled skull thresholds. | `npc.cpp`, `monster.cpp`, `itemevents.cpp`, `guild.h`, `item.cpp`, `luascript.cpp` |
| House failure handling | Preserve the current world owner on failed SQL updates, defer rent decisions when a player cannot load, and propagate transfer/save failures. Full durability is not claimed, see below. | `house.cpp` |
| Lua content | Check payment and teleport outcomes, correct shop-container handling and rune APIs, fix chat lifecycle and reject placeholder store rewards. | `data/lib/core/player.lua`, `data/npc/lib/npcsystem/modules.lua`, `data/scripts/spells/runes/support`, `data/chatchannels/scripts/lua_channel.lua`, `data/scripts/store/store.lua` |
| Monster registration | Give eight conflicting variants unique registry names while preserving display names and stats, so one variant cannot replace another type. | `data/scripts/monsters/monsters` |
| Startup and CI | Wait for loader completion with a predicate, preserve validated command-line overrides, display invalid CLI errors, clamp long-name formatting, and require passing tests before release build jobs. | `otserv.cpp`, `.github/workflows` |

## Verification

Both the normal server and complete engine test executable built with GCC 16.2
on Windows, using external temporary CMake orchestration for the existing source
set. No CMake project or downloaded dependency was added to this repository.
Native Linux build, unit-test and data workflows subsequently passed on
`b6c8bdb`. The Windows MSVC build exposed an unsupported constant-evaluation call
to `std::isnan` in `CustomSkill::safeRound`. The helper now runs normally, with
the same NaN/overflow checks and rounding. Its in-class definition remains inline.
The Windows workflow publishes a test report only when one exists, while build
and test failures still fail the job. The corrected MSVC run is tracked in
[PR #272](https://github.com/Black-Tek/BlackTek-Server/pull/272/checks).

| Check | Result |
| --- | --- |
| Complete engine suite | 170 tests pass. Ten shuffled repetitions also pass, 1,700 test executions. |
| Network, Lua, item, persistence and market boundary suites | 77/77 pass. The same assertions expose 66 failures in the original methods. |
| House, trade and zone-registry failure suites | 7/7, 4/4 and 4/4 pass. Scripts are included and also wired into Linux CI. |
| Native Game/Player/Zone comparisons | 26 focused engine tests pass. Eighteen targeted original-method comparisons fail, including five crashes. These comparisons retain unrelated fixes and matching class layouts, rather than building a mismatched ABI. |
| Process teardown with a pooled item retained by a timer | Original Game destructor crashes with an access violation; corrected destructor exits successfully. |
| Data suite | All suites pass, including compilation of 1,287 Lua files, XML/TOML parsing, 740 monster registration mocks, content references and sample-world integrity. |
| Startup executable | Help/version, missing port, nonnumeric port, out-of-range port and unavailable database checks all behave as expected without a crash or hang. |
| Other checks | Workflow YAML parsing, shipped RSA-key consistency, outfit/mount ID checks and `git diff --check` pass. |

The repeated suite initially exposed a test fixture that assumed removing a map
tile erased its slot. The engine intentionally retains the empty slot. The test
now validates and reuses that empty fixture tile; no map behavior was changed to
make the test pass.

Combat microbenchmark: 225 tiles, 100 warm-up casts and five runs of 1,000 casts
per case. Correct target counts were checked. These are local synthetic timings,
not production server throughput or tail-latency measurements.

| Targets | Before, no mitigation | After, no mitigation | Before, mitigation | After, mitigation |
| --- | ---: | ---: | ---: | ---: |
| 8 | 40.835 us | 41.194 us | 41.454 us | 40.084 us |
| 32 | 48.396 us | 48.934 us | 49.972 us | 50.537 us |
| 128 | 85.172 us | 87.471 us | 90.882 us | 88.447 us |

Measured changes range from about 3.3% faster to 2.7% slower. This supports a
correctness fix without a large regression in these cases, not a general speedup.

## Outstanding issues and limits

- **High priority, market durability:** offer/history/bank writes are checked,
  but database settlement and inventory delivery/player saving are not one
  crash-atomic operation. A durable settlement/outbox design and crash-recovery
  integration tests are still needed. Automatic database reconnects and ambiguous
  COMMIT outcomes also need explicit transaction-level handling.
- **High priority, house durability:** an owner update can succeed in SQL before
  later evacuation fails. Partial moves and rent debit/paid-until persistence are
  not rolled back atomically. Failure guards in this patch do not solve that
  larger transaction boundary.
- **High priority, authentication session lifetime:** game authentication still
  trusts the client-carried token timestamp under the existing session policy.
  Previously valid token sessions can outlive the intended TOTP window. Use a
  server-issued expiring session with deliberate character-list and login-queue
  semantics; simply enforcing a short TOTP timeout would break those flows.
- **Item reload:** failed DAT parsing preserves old definitions, but a later
  TOML failure can still leave partial reload state. Skill/buff registries also
  need an atomic reload design with explicit clearing/replacement.
- **Trade callbacks:** arbitrary Lua side effects cannot be undone automatically.
  Ordinary second-leg transfer failure is recovered; if both original-location
  and inbox recovery fail, a logged recovery error still requires attention.
- **IPv6:** IP storage and bans remain IPv4-based. Native IPv6 is rejected safely,
  not fully supported.
- **Monster reload:** definitions removed from disk are retained while live
  monsters may reference them. This avoids dangling pointers but needs a separate
  retirement policy for fully reclaiming removed definitions.
- **Build portability:** Docker/bootstrap files retain preexisting toolchain and
  configuration problems, including older compiler targets, Docker configuration
  packaging/database defaults and ARM bootstrap selection. Those installation
  paths were not rebuilt or certified here.
- **Inactive examples:** legacy `getMonster`, disabled market-look and unused
  combat-ring helpers remain in inactive content. They were not silently enabled
  or rewritten as gameplay changes.
- **Unconfirmed follow-ups:** metrics query/output behavior, console queue
  saturation during shutdown and raid/global-event reload reentrancy merit
  targeted runtime investigation. They are not reported as reproduced defects.
- No live MySQL settlement/restart tests, representative production-map load,
  real-client multiplayer sessions, full-engine sanitizer soak, exhaustive packet
  fuzzing or every-script manual gameplay review was completed. Local boundary
  tests are not substitutes for those checks.

## Existing spawn pull request

[PR #270](https://github.com/Black-Tek/BlackTek-Server/pull/270) contains the
separate spawn interval/re-entry fix. This review does not duplicate its policy
changes. Both touch `zones.cpp` and `zones.h`, so maintainers should reconcile and
rerun both regression sets when merging. The new zone fixes address ownership,
callback registry invalidation, IDs and conversion failures.

## Reproducing tests

See [tests/README.md](../tests/README.md) for the native Test configuration and
standalone failure runners, and [tests/data/README.txt](../tests/data/README.txt)
for Lua/data checks. Keep generated executables, logs, test maps and result files
outside the checkout. The CSV ledger covers the source snapshot before this
report and its ledger were added, and records hashes for that snapshot.
