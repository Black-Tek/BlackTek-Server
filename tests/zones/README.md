# Spawn coroutine regression tests

These focused tests run the actual zone coroutines without starting a server,
opening a database or loading a map. They supplement the full engine Google Test
build described in [../README.md](../README.md).

Requirements: Python 3 and a C++20 compiler. No additional packages are needed.

```sh
python3 tests/zones/run_spawn_tests.py --cxx g++
python3 tests/zones/run_spawn_tests.py --cxx clang++ --sanitize
```

On Windows, a compiler command such as `--cxx "zig c++"` is also supported.
Use `--build-dir build/spawn-tests` to retain the generated translation unit and
executable, or omit it to clean up temporary build outputs automatically.

`--sanitize` enables ASan and UBSan with recovery disabled, so a sanitizer error
fails its scenario. The available Windows Zig toolchain provides working UBSan
checks but does not provide effective AddressSanitizer instrumentation, despite
accepting its flag. The Ubuntu CI job runs both ASan and UBSan with GCC.

To reproduce the regression using an earlier revision while retaining these
tests:

```sh
python3 tests/zones/run_spawn_tests.py --cxx g++ --source-ref origin/master
```

`--case relative-stay` runs one named scenario. Each scenario runs in a separate
process, so a coroutine crash or timeout cannot prevent the remaining checks.

## What is exercised

- Actual `Zone` declarations, spawn and event-loop methods, `SpawnCoroTask`
  promise, `EventAwaiter`, `ZoneOverlay::Trigger`, `TimerQueue` and `SleepFor`.
- Fixed and relative monster intervals, death and despawn, repeated player
  exit/reentry before and after the deadline, and remaining players.
- Passive and forced policies, immediate initial population, zero-interval
  fallback, duplicate membership and paused-entry queue retention.
- Fixed, relative, staged and triggered boss cooldowns, entering or leaving
  during cooldown, delayed master placement and deferred relative placement.
- Dead minions remaining queued for the next boss cycle and cancelled timers
  after zone removal.
- Death followed by despawn, pending master retries after minion deaths, and
  deferred minions whose placement deadline arrives during boss cooldown.
- One hundred boss cycles with repeated player movement, checking that live
  coroutine frames, timer entries and placed creatures do not accumulate.

The Python runner extracts those implementation sections from the chosen Git
revision. It changes only `TimerQueue`'s clock alias to a deterministic clock and
omits a header-only `#pragma once` when concatenating the test translation unit.
The tests do not contain copies of the spawn algorithms. Advancing the clock
resumes the real coroutines at each scheduled deadline, without wall-clock sleeps.

World placement, creature identity, random draws and zone lookup are small test
doubles. Inventory, Lua events, real map occupancy, database operations, the
production dispatcher thread and zone-pool destruction are outside this fixture.
Run the normal engine build and integration checks as well. The `.cpp.in` suffix
keeps this standalone fixture out of premake's `tests/**.cpp` Google Test glob.
