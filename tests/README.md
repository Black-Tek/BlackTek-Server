# Unit Tests

Google Test suite for BlackTek-Server. Tests live alongside the engine
sources and are built with the same premake-driven build the server
itself uses. There is no separate CMake project in the repository.

## Building

The `Test` configuration is exclusive: it swaps the server's normal
entry point (`src/otserv.cpp`'s `main`) for Google Test's own via the
`BLACKTEK_UNIT_TESTS` compile define, so the resulting binary is the
test runner, not the game server.

```sh
# one-time: vcpkg install --x-feature=tests (pulls in gtest)
premake5 gmake2
make config=test_64      # or test_arm64 / test_arm
./Black-Tek-Server       # runs the gtest suite
```

`Debug`/`Release` builds are unaffected: `tests/**.cpp` is only added to
the file list, and `BLACKTEK_UNIT_TESTS` is only defined, under
`configurations:Test` in `premake5.lua`.

## Layout

Tests are grouped into one subfolder per system, mirroring the
eventual `src/` folder-by-system grouping:

```
tests/
  combat/
    combat_test.cpp         # Combat's own formulas, accessors, damage modifiers
    simd_kernels_test.cpp   # the scalar/SSE2/SSE4.1/AVX2 kernels in simd_kernels.h
```

Add new systems as their own subfolder (e.g. `tests/items/`,
`tests/creatures/`) rather than dropping files at the top level.
Premake picks up any new file automatically via the `tests/**.cpp`
glob in `premake5.lua`, so no build changes are needed to add a subfolder
or a file within one. Lua content has a separate suite under `tests/data`;
native Lua API regressions are also tested through engine fixtures.

## Writing tests

- Prefer testing pure/static logic (formulas, lookups, helpers that
  don't require a running game world) directly. See
  `combat/combat_test.cpp` for the pattern used against `Combat`'s
  static formula evaluators.
- For a vectorized (SSE2/SSE4.1/AVX2) code path, don't just dispatch
  through the public wrapper and hope. Force `BlackTek::SIMD::g_level`
  to each level in turn (see `ScopedSimdLevel` in
  `combat/simd_kernels_test.cpp`) and diff every level's output against
  scalar *and* against the single-target formula it's supposed to
  reproduce. That approach caught 4 real, reproducible bugs in
  `src/simd_dispatch.h` / `src/simd_kernels.h`, since fixed.
- Use isolated engine fixtures for behavior that needs Player, Monster,
  Map or Game state. Restore globals and empty temporary map tiles after
  each test so shuffled and repeated runs remain independent. These
  fixtures do not replace live database and client integration tests.
- `BlackTek::Build::IsUnitTestBuild` (`src/build_config.h`) is available
  if a piece of production code ever needs to branch on whether it was
  compiled into the test binary.

## Runtime and failure regressions

The suite also constructs isolated Player, Monster, Item, Party, Lua and Zone
fixtures. Tests use temporary content directories and never connect to a live
database or modify the shipped map. New coverage is grouped under `core`,
`network`, `persistence`, `runtime` and `world`.

The `run_*standalone` scripts compile selected production methods verbatim with
deterministic boundaries. They exercise failures that are difficult to trigger
in a normal game session, including failed SQL operations and reentrant Lua
callbacks. These are boundary tests, not a substitute for live database or
client integration tests. Use an output directory outside the checkout.

```sh
python3 tests/persistence/run_house_standalone.py --compiler g++ --output-directory /tmp/blacktek-house-tests
python3 tests/world/run_trade_standalone.py --compiler g++ --output-directory /tmp/blacktek-trade-tests
python3 tests/world/run_zone_registry_standalone.py --compiler g++ --output-directory /tmp/blacktek-zone-tests
python3 tests/data/run.py
```

Windows PowerShell runners under `network` and `persistence` accept `-Compiler`
and `-OutputDirectory`. Use a GCC-compatible compiler; the Lua runner additionally
expects LuaJIT headers and its library in that compiler's installation prefix.
`-BaselineRef` (PowerShell) or `--baseline-ref` (Python) runs the same assertions
against an earlier Git revision to demonstrate the original failures.

## CI

Unit tests do not run inline in the `Linux Build`/`Windows Build`
runners. Each triggers its own dedicated reusable workflow,
`.github/workflows/linux_unit_tests.yml` and
`.github/workflows/windows_unit_tests.yml` — which builds the `Test`
configuration, runs it with JUnit XML output, and publishes a
[dorny/test-reporter](https://github.com/dorny/test-reporter) check
with a per-test pass/fail breakdown instead of raw console output.
Release build jobs wait for the corresponding unit-test job to pass.
