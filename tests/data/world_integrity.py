"""Check map teleports and active raid locations without launching the server."""
from pathlib import Path
import struct
import tomllib
import unittest

ROOT = Path(__file__).resolve().parents[2]

def parse_map(path):
    data = path.read_bytes()
    nodes, stack = [], []
    i = 4
    while i < len(data):
        byte = data[i]
        i += 1
        if byte == 254:
            node = {'type': data[i], 'props': bytearray(), 'children': []}
            i += 1
            (stack[-1]['children'] if stack else nodes).append(node)
            stack.append(node)
        elif byte == 255:
            stack.pop()
        elif byte == 253:
            stack[-1]['props'].append(data[i])
            i += 1
        else:
            stack[-1]['props'].append(byte)
    assert not stack and len(nodes) == 1
    tiles, teleports = set(), []
    widths = {3:4,4:2,5:2,8:5,9:2,10:2,12:1,14:1,15:1,16:4,17:1,18:4,20:4,21:4,22:2,27:4,28:4,29:4,30:4,31:4,32:1,33:1,35:4,36:2,37:1,38:4,43:2,45:4,46:1}
    strings = {1,2,6,7,11,13,19,24,25,26,41,42}
    def visit(node, area=None, position=None):
        props, kind = node['props'], node['type']
        if kind == 4:
            area = struct.unpack('<HHB', props)
        elif kind in (5, 14):
            position = (area[0] + props[0], area[1] + props[1], area[2])
            tiles.add(position)
        elif kind == 6 and struct.unpack_from('<H', props)[0] == 1387:
            offset = 2
            while offset < len(props):
                attribute = props[offset]
                offset += 1
                if attribute == 0:
                    break
                if attribute == 8:
                    teleports.append((position, struct.unpack_from('<HHB', props, offset)))
                if attribute in widths:
                    offset += widths[attribute]
                elif attribute in strings:
                    offset += 2 + struct.unpack_from('<H', props, offset)[0]
                else:
                    raise AssertionError(f'Unsupported teleport attribute {attribute}')
        for child in node['children']:
            visit(child, area, position)
    visit(nodes[0])
    return tiles, teleports

class WorldIntegrity(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tiles, cls.teleports = parse_map(ROOT / 'data/world/forgotten.otbm')

    def test_teleport_destinations_exist(self):
        self.assertGreater(len(self.teleports), 0)
        self.assertEqual([(p,d) for p,d in self.teleports if d not in self.tiles], [])

    def test_examples_are_not_active_raids(self):
        self.assertFalse((ROOT / 'data/raids/example_raids.toml').exists())
        self.assertTrue((ROOT / 'data/raids/example_raids.toml.example').exists())

    def test_active_single_spawn_positions_exist(self):
        for path in (ROOT / 'data/raids').rglob('*.toml'):
            for raid in tomllib.loads(path.read_text(encoding='utf-8')).values():
                for event in raid.get('events', []):
                    if event.get('type') == 'singlespawn':
                        position = event.get('position') or [event['x'],event['y'],event['z']]
                        self.assertIn(tuple(position), self.tiles, str(path))

if __name__ == '__main__':
    unittest.main()
