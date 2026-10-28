"""Run isolated data regressions and compile/parse the complete data tree.

Usage: python tests/data/run.py [--lua-library path/to/lua.dll]
A Lua 5.3+ executable on PATH can be used instead of a shared library.
No server, database, or game client is started.
"""
from __future__ import annotations
import argparse
import ctypes
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import tomllib
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[2]
TESTS = Path(__file__).resolve().parent

class LuaLibrary:
    def __init__(self, path):
        self.lib = ctypes.CDLL(str(Path(path).resolve()))
        lib = self.lib
        lib.luaL_newstate.restype = ctypes.c_void_p
        lib.lua_close.argtypes = [ctypes.c_void_p]
        lib.luaL_loadfilex.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
        lib.lua_pcallk.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_ssize_t, ctypes.c_void_p]
        lib.lua_tolstring.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ctypes.c_size_t)]
        lib.lua_tolstring.restype = ctypes.c_char_p
        if hasattr(lib, 'luaL_openlibs'):
            lib.luaL_openlibs.argtypes = [ctypes.c_void_p]
            self.open = lambda state: lib.luaL_openlibs(state)
        else:
            lib.luaL_openselectedlibs.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
            self.open = lambda state: lib.luaL_openselectedlibs(state, 0x7fffffff, 0)

    def run(self, path, execute=True):
        lib = self.lib
        state = lib.luaL_newstate()
        if not state:
            raise RuntimeError('Cannot allocate a Lua state')
        try:
            if execute:
                self.open(state)
            status = lib.luaL_loadfilex(state, str(path).encode(), None)
            if not status and execute:
                status = lib.lua_pcallk(state, 0, 0, 0, 0, None)
            if status:
                raise RuntimeError(lib.lua_tolstring(state, -1, None).decode(errors='replace'))
        finally:
            lib.lua_close(state)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--lua-library', default=os.getenv('BLACKTEK_LUA_LIBRARY'))
    args = parser.parse_args()
    os.chdir(ROOT)
    library = LuaLibrary(args.lua_library) if args.lua_library else None
    executable = next((shutil.which(name) for name in ['lua5.4', 'lua5.3', 'lua'] if shutil.which(name)), None)
    if not library and not executable:
        parser.error('Install Lua 5.3+ or supply --lua-library')
    files = sorted((ROOT / 'data').rglob('*.lua'))
    if library:
        for path in files:
            library.run(path, execute=False)
    else:
        body = '\n'.join('assert(loadfile([==[' + path.as_posix() + ']==]))' for path in files)
        with tempfile.TemporaryDirectory() as folder:
            script = Path(folder) / 'compile_data.lua'
            script.write_text(body, encoding='utf-8')
            subprocess.run([executable, str(script)], cwd=ROOT, check=True)
    print(f'PASS: compiled {len(files)} Lua files', flush=True)
    for path in (ROOT / 'data').rglob('*.xml'):
        ET.parse(path)
    for path in (ROOT / 'data').rglob('*.toml'):
        tomllib.loads(path.read_text(encoding='utf-8-sig'))
    print('PASS: parsed all XML and TOML files', flush=True)
    for path in sorted(TESTS.glob('*.lua')):
        print(f'Running {path.relative_to(ROOT)}', flush=True)
        if library:
            library.run(path)
        else:
            subprocess.run([executable, str(path)], cwd=ROOT, check=True)
    for path in sorted(TESTS.glob('*.py')):
        if path.name != 'run.py':
            print(f'Running {path.relative_to(ROOT)}', flush=True)
            subprocess.run([sys.executable, str(path)], cwd=ROOT, check=True)
    print('PASS: all data regression suites', flush=True)

if __name__ == '__main__':
    main()
