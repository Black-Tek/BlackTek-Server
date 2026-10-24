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
    combat_test.cpp         # Combat's own formulas, accessors, damage modifiers
    simd_kernels_test.cpp   # the scalar/SSE2/SSE4.1/AVX2 kernels in simd_kernels.h
```

Add new systems as their own subfolder (e.g. `tests/items/`,
`tests/creatures/`) rather than dropping files at the top level.
Premake picks up any new file automatically via the `tests/**.cpp`
glob in `premake5.lua` — no build changes needed to add a subfolder
or a file within one. The Lua scripting interface is explicitly out
of scope for these folders — it gets its own test suite instead.

## Writing tests

- Prefer testing pure/static logic (formulas, lookups, helpers that
  don't require a running game world) directly — see
  `combat/combat_test.cpp` for the pattern used against `Combat`'s
  static formula evaluators.
- For a vectorized (SSE2/SSE4.1/AVX2) code path, don't just dispatch
  through the public wrapper and hope — force `BlackTek::SIMD::g_level`
  to each level in turn (see `ScopedSimdLevel` in
  `combat/simd_kernels_test.cpp`) and diff every level's output against
  scalar *and* against the single-target formula it's supposed to
  reproduce. That approach caught 4 real, reproducible bugs in
  `src/simd_dispatch.h` / `src/simd_kernels.h`, since fixed.
- Some functions are deliberately not covered yet: anything that needs
  a live Player/Monster/Map/Game (`Combat::strike_target`,
  `post_damage`, the `target()` overloads, `postCombatEffects`, etc.)
  is out of scope until there's a lightweight way to construct that
  state for a test.
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
