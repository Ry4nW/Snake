import unittest

from snake_game.ai.pathfinding import astar_path, bfs_path, direction_between, flood_fill_count


class BfsPathTests(unittest.TestCase):
    def test_finds_the_shortest_path_on_an_open_grid(self):
        path = bfs_path((0, 0), (2, 0), blocked=set(), cols=5, rows=5)
        self.assertEqual(path, [(0, 0), (1, 0), (2, 0)])

    def test_routes_around_obstacles(self):
        blocked = {(1, 0), (1, 1)}
        path = bfs_path((0, 0), (2, 0), blocked, cols=3, rows=3)
        self.assertIsNotNone(path)
        self.assertTrue(all(cell not in blocked for cell in path))

    def test_returns_none_when_the_goal_is_walled_off(self):
        blocked = {(1, y) for y in range(5)}
        path = bfs_path((0, 0), (4, 0), blocked, cols=5, rows=5)
        self.assertIsNone(path)


class AStarPathTests(unittest.TestCase):
    def test_matches_bfs_path_length_on_an_open_grid(self):
        bfs = bfs_path((0, 0), (4, 4), set(), cols=5, rows=5)
        astar = astar_path((0, 0), (4, 4), set(), cols=5, rows=5)
        self.assertEqual(len(bfs), len(astar))

    def test_returns_none_when_unreachable(self):
        blocked = {(1, y) for y in range(5)}
        self.assertIsNone(astar_path((0, 0), (4, 0), blocked, cols=5, rows=5))


class FloodFillTests(unittest.TestCase):
    def test_counts_an_enclosed_region_correctly(self):
        # wall at col 2 splits 5x5 grid into two pockets
        blocked = {(2, y) for y in range(5)}
        count = flood_fill_count((0, 0), blocked, cols=5, rows=5)
        self.assertEqual(count, 10)

    def test_returns_zero_when_start_is_blocked(self):
        self.assertEqual(flood_fill_count((0, 0), blocked={(0, 0)}, cols=5, rows=5), 0)


class DirectionBetweenTests(unittest.TestCase):
    def test_returns_the_unit_step_between_adjacent_cells(self):
        self.assertEqual(direction_between((3, 3), (4, 3)), (1, 0))
        self.assertEqual(direction_between((3, 3), (3, 2)), (0, -1))


if __name__ == '__main__':
    unittest.main()
