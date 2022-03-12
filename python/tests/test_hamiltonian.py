import unittest

from snake_game.ai.hamiltonian import build_hamiltonian_cycle


class HamiltonianCycleTests(unittest.TestCase):
    def test_visits_every_cell_exactly_once(self):
        cols, rows = 6, 4
        cycle = build_hamiltonian_cycle(cols, rows)
        self.assertEqual(len(cycle), cols * rows)
        self.assertEqual(len(set(cycle)), cols * rows)

    def test_consecutive_cells_are_grid_adjacent_including_the_wraparound(self):
        cols, rows = 6, 4
        cycle = build_hamiltonian_cycle(cols, rows)
        for a, b in zip(cycle, cycle[1:] + cycle[:1]):
            manhattan_distance = abs(a[0] - b[0]) + abs(a[1] - b[1])
            self.assertEqual(manhattan_distance, 1)

    def test_matches_the_actual_game_board_dimensions(self):
        cycle = build_hamiltonian_cycle(18, 12)
        self.assertEqual(len(cycle), 18 * 12)

    def test_rejects_a_grid_with_an_odd_number_of_cells(self):
        with self.assertRaises(ValueError):
            build_hamiltonian_cycle(3, 3)


if __name__ == '__main__':
    unittest.main()
