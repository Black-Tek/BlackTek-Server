#!/usr/bin/env python3
"""Compile real zone coroutines with a deterministic clock and small world doubles.

No third-party Python packages or game data/database are required. The fixture is
not an alternative spawn implementation: Zone declarations, coroutine methods,
promise, awaiter and timer queue are read directly from the selected source tree.
"""

import argparse
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default=os.environ.get("CXX", "c++"),
                        help='C++20 compiler command, e.g. "clang++" or "zig c++"')
    parser.add_argument("--source-ref", help="Read engine sources from this Git revision")
    parser.add_argument("--build-dir", type=Path,
                        help="Keep generated fixture/binary here instead of a temporary directory")
    parser.add_argument("--case", action="append", help="Run only a named case (repeatable)")
    parser.add_argument("--sanitize", action="store_true", help="Enable ASan and UBSan where supported")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]

    def read(relative):
        if args.source_ref:
            return subprocess.check_output(["git", "show", f"{args.source_ref}:{relative}"],
                                           cwd=root, text=True, encoding="utf-8")
        return (root / relative).read_text(encoding="utf-8")

    def between(text, start, end):
        first = text.index(start)
        return text[first:text.index(end, first)]

    header, implementation, game = read("src/zones.h"), read("src/zones.cpp"), read("src/game.h")
    zone_types = between(header, "    struct ZoneTag", "    class ZonePool")
    overlay = between(header, "    class ZoneOverlay", "    class ZoneManager")
    selection = between(implementation, "    struct Offset", "    static std::optional<Position> ParseXyz")
    coroutines = between(implementation, "    void ZoneOverlay::Trigger", "    size_t ZoneManager::Count()")
    timers = between(game, "struct TimerQueue", "struct Expirable")
    clock_alias = "using Clock = std::chrono::steady_clock;"
    if timers.count(clock_alias) != 1:
        raise RuntimeError("TimerQueue clock changed; review deterministic-clock substitution")
    timers = timers.replace(clock_alias, "using Clock = BlackTek::Tests::FakeClock;")
    template = (root / "tests/zones/spawn_fixture.cpp.in").read_text(encoding="utf-8")
    sources = {"HANDLE": read("src/handle.h").replace("#pragma once", ""), "POSITION": read("src/position.h"),
               "ZONE_TYPES": zone_types + overlay, "SELECTION": selection, "COROUTINES": coroutines, "TIMERS": timers}
    for marker, source in sources.items():
        template = template.replace(f"@{marker}@", source)

    cases = ["fixed-stay", "relative-stay", "fixed-despawn", "relative-despawn", "fixed-reenter-before", "relative-reenter-before",
             "fixed-reenter-after", "relative-reenter-after", "passive-death-occupied",
             "passive-despawn-occupied", "passive-after-deadline", "forced-occupied",
             "passive-player-death-before-deadline",
             "forced-last-player-leaves", "fallback-zero", "initial-startup",
             "manual-pause-queue", "player-death-reentry", "duplicate-enter", "duplicate-forced-enter", "second-player-stays",
             "failed-placement", "boss-fixed-stay", "boss-relative-stay", "boss-staged-stay",
             "boss-fixed-reentry", "boss-relative-reentry", "boss-staged-reentry",
             "boss-fixed-empty", "boss-relative-empty", "boss-triggered-cooldown",
             "boss-placement-retry", "boss-relative-retry-empty", "boss-minion-reentry",
             "boss-fixed-death-despawn", "boss-relative-death-despawn", "boss-staged-death-despawn",
             "boss-deferred-minion-next-cycle", "boss-late-deferred-minion", "boss-late-minion-player-stays",
             "boss-late-minion-player-returns", "boss-fixed-pending-master", "boss-relative-pending-master",
             "boss-repeat-movement", "deleted-zone-timer"]
    if args.case:
        unknown = set(args.case) - set(cases)
        if unknown:
            parser.error("unknown case(s): " + ", ".join(sorted(unknown)))
        cases = args.case
    temporary = None
    if args.build_dir:
        build = args.build_dir.resolve()
        build.mkdir(parents=True, exist_ok=True)
    else:
        temporary = tempfile.TemporaryDirectory(prefix="blacktek-spawn-tests-")
        build = Path(temporary.name)
    try:
        source = build / "spawn_fixture.cpp"
        binary = build / ("spawn_tests.exe" if os.name == "nt" else "spawn_tests")
        source.write_text(template, encoding="utf-8")
        compiler = shlex.split(args.cxx, posix=os.name != "nt")
        compiler = [part.strip('"') for part in compiler]
        command = compiler + ["-std=c++20", "-O0", "-g", str(source), "-o", str(binary)]
        if args.sanitize:
            command += ["-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer"]
        subprocess.run(command, cwd=root, check=True)
        failures = []
        for case in cases:
            try:
                result = subprocess.run([str(binary), case], cwd=root, timeout=10, capture_output=True, text=True)
            except subprocess.TimeoutExpired:
                failures.append(case)
                print(f"FAIL {case} (timeout)")
                continue
            if result.returncode:
                failures.append(case)
                print(f"FAIL {case} (exit {result.returncode})")
                if result.stderr:
                    print(result.stderr.rstrip())
            else:
                print(f"PASS {case}")
        print(f"Spawn coroutine regressions: {len(cases) - len(failures)} passed, {len(failures)} failed")
        return bool(failures)
    finally:
        if temporary:
            temporary.cleanup()


if __name__ == "__main__":
    sys.exit(main())
