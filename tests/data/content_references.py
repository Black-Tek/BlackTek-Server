"""Run from any directory with Python 3.11+: python tests/data/content_references.py."""

import pathlib
import re
import tomllib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]
DATA = ROOT / "data"


def read_toml(path):
    with path.open("rb") as stream:
        return tomllib.load(stream)


class ContentReferences(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.items = read_toml(DATA / "items/items.toml")["items"]
        cls.item_ids = set()
        for item in cls.items:
            start = item.get("id", item.get("fromid"))
            end = item.get("id", item.get("toid"))
            cls.item_ids.update(range(start, end + 1))
        cls.augments = [
            (path, name, augment)
            for path in (DATA / "augments").rglob("*.toml")
            for name, augment in read_toml(path).items()
        ]

    def test_item_augment_references_resolve_exactly(self):
        names = {augment["name"] for _, _, augment in self.augments}
        for item in self.items:
            references = item.get("augment", [])
            if isinstance(references, str):
                references = [references]
            for name in references:
                with self.subTest(item=item.get("id"), augment=name):
                    self.assertIn(name, names)

    def test_augment_damage_filters_and_chances_are_valid(self):
        # Unknown damage strings are interpreted as unrestricted COMBAT_NONE.
        damage_names = {
            "none", "all", "physical", "melee", "energy", "electric",
            "earth", "poison", "fire", "lifedrain", "lifesteal", "lifeleech",
            "manadrain", "manasteal", "manaleech", "drown", "water", "ice",
            "holy", "death", "curse",
        }
        for path, name, augment in self.augments:
            for modifier in augment.get("modifiers", []):
                with self.subTest(file=path.name, augment=name, modifier=modifier):
                    self.assertIn(modifier.get("damage", "none"), damage_names)
                    if "chance" in modifier:
                        self.assertIs(type(modifier["chance"]), int)
                        self.assertGreaterEqual(modifier["chance"], 0)
                        self.assertLessEqual(modifier["chance"], 100)

    def test_monster_summon_names_resolve(self):
        sources = {
            path: path.read_text(encoding="utf-8")
            for path in (DATA / "scripts/monsters").rglob("*.lua")
            if "#" not in path.name
        }
        # Current monster registrations and summon entries use literal names.
        names = {
            name.lower()
            for source in sources.values()
            for name in re.findall(r'Game\.createMonsterType\("([^\"]+)"\)', source)
        }
        self.assertTrue(names, "No monster registrations were inspected")
        for path, source in sources.items():
            for block in re.findall(r"monster\.summons\s*=\s*\{(.*?)\n\}", source, re.S):
                for name in re.findall(r'name\s*=\s*"([^\"]+)"', block):
                    with self.subTest(file=path.name, summon=name):
                        self.assertTrue(name.lower() in names, f"Unknown summon {name!r} in {path.name}")

    def test_enabled_conjuring_spells_create_defined_items(self):
        for path in (DATA / "scripts/spells").rglob("*.lua"):
            if "#" in path.name:
                continue  # Matches Scripts::loadScripts' disabled-file convention.
            for output in re.findall(r"conjureItem\(\s*\d+\s*,\s*(\d+)", path.read_text()):
                with self.subTest(file=str(path.relative_to(DATA)), item=output):
                    self.assertIn(int(output), self.item_ids)


if __name__ == "__main__":
    unittest.main()
