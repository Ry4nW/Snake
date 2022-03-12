import json
import os
import tempfile
import unittest

from snake_game.highscore import HighScoreStore


class HighScoreStoreTests(unittest.TestCase):
    def setUp(self):
        fd, self.path = tempfile.mkstemp(suffix='.json')
        os.close(fd)
        os.remove(self.path)  # start with no save file

    def tearDown(self):
        if os.path.exists(self.path):
            os.remove(self.path)

    def test_defaults_to_zero_when_no_file_exists(self):
        store = HighScoreStore(self.path)
        self.assertEqual(store.value, 0)

    def test_update_persists_a_new_high_score_to_disk(self):
        store = HighScoreStore(self.path)
        store.update(12)
        with open(self.path) as f:
            self.assertEqual(json.load(f)['high_score'], 12)

    def test_update_ignores_a_lower_score(self):
        store = HighScoreStore(self.path)
        store.update(12)
        store.update(5)
        self.assertEqual(store.value, 12)

    def test_a_new_store_reloads_the_persisted_value(self):
        store = HighScoreStore(self.path)
        store.update(9)
        reloaded = HighScoreStore(self.path)
        self.assertEqual(reloaded.value, 9)


if __name__ == '__main__':
    unittest.main()
