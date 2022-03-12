import unittest

from snake_game.food import FoodManager


class SmallestCellRng:
    """fake random.choice, always picks smallest cell so spawns are predictable"""

    def choice(self, seq):
        return sorted(seq)[0]


class FoodManagerTests(unittest.TestCase):
    def test_reset_spawns_the_configured_count(self):
        food = FoodManager(cols=3, rows=3, count=2, rng=SmallestCellRng())
        food.reset(occupied={(0, 0)})
        self.assertEqual(len(food.items), 2)

    def test_reset_never_spawns_on_an_occupied_cell(self):
        food = FoodManager(cols=3, rows=3, count=2, rng=SmallestCellRng())
        food.reset(occupied={(0, 0)})
        self.assertNotIn((0, 0), food.items)

    def test_consume_removes_the_eaten_item_and_respawns_a_new_one(self):
        food = FoodManager(cols=3, rows=3, count=1, rng=SmallestCellRng())
        food.reset(occupied=set())
        eaten = food.items[0]

        consumed = food.consume(eaten, occupied={eaten})

        self.assertTrue(consumed)
        self.assertEqual(len(food.items), 1)
        self.assertNotIn(eaten, food.items)

    def test_consume_ignores_a_cell_with_no_food(self):
        food = FoodManager(cols=3, rows=3, count=1, rng=SmallestCellRng())
        food.reset(occupied=set())
        self.assertFalse(food.consume((2, 2), occupied=set()))


if __name__ == '__main__':
    unittest.main()
