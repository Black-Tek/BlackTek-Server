Data regression checks

Run from any working directory:
  python tests/data/run.py

Requirements: Python 3.11+ and Lua 5.3+ on PATH. On Windows with the server's
Lua shared library, use:
  python tests/data/run.py --lua-library C:/path/to/lua.dll

The runner compiles all data Lua files without executing their top-level code,
parses XML/TOML, runs isolated Lua callback tests with engine mocks, and checks
item/augment/monster/map references. Each Lua suite receives a fresh Lua state.
These tests do not start the game server or connect to a database.

The data_regressions GitHub workflow runs these checks with Lua 5.4 and Python
3.11. Engine compilation and live multiplayer tests remain separate checks.

Native API changes
Movement validation requires Tile:queryAdd, and complete modal cleanup needs
Player:forgetModalWindow. Both require the updated native bindings.
Deploy the rebuilt engine together with these data files. Copying only the
updated data files beside an older executable is not supported.

Unavailable content
Wand of Darkness still needs item 28416 and corresponding client assets.
Versperoth still needs its intended minion definition. These unresolved content
references are disabled with comments rather than replaced with unrelated
assets or monsters. Three door assets without open counterparts are not keyed
doors. The example raids remain disabled until a map author chooses valid spawn
locations. Five broken map portals were removed at these coordinates:
(122,86,10), (304,74,3), (455,446,8), (136,553,3), (182,669,13).
