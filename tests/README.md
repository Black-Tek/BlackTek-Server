# Unit Tests

Google Test suite for BlackTek-Server. Tests live alongside the engine
sources and are built with the same premake-driven build the server
itself uses — there is no separate CMake project.

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
    combat_test.cpp
```

Add new systems as their own subfolder (e.g. `tests/items/`,
`tests/creatures/`) rather than dropping files at the top level.
Premake picks up any new file automatically via the `tests/**.cpp`
glob in `premake5.lua` — no build changes needed to add a subfolder
or a file within one.

## Writing tests

- Prefer testing pure/static logic (formulas, lookups, helpers that
  don't require a running game world) directly — see
  `combat/combat_test.cpp` for the pattern used against `Combat`'s
  static formula evaluators.
- `BlackTek::Build::IsUnitTestBuild` (`src/build_config.h`) is available
  if a piece of production code ever needs to branch on whether it was
  compiled into the test binary.

## CI

Unit tests do not run inline in the `Linux Build`/`Windows Build`
runners. Each triggers its own dedicated reusable workflow —
`.github/workflows/linux_unit_tests.yml` and
`.github/workflows/windows_unit_tests.yml` — which builds the `Test`
configuration, runs it with JUnit XML output, and publishes a
[dorny/test-reporter](https://github.com/dorny/test-reporter) check
with a per-test pass/fail breakdown instead of raw console output.
