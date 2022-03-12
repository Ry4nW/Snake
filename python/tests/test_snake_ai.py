import unittest

from snake_game.ai.snake_ai import SnakeAI
from snake_game.snake import RIGHT, Snake


class SnakeAITests(unittest.TestCase):
    def test_heads_toward_food_on_an_open_board(self):
        ai = SnakeAI(cols=10, rows=10)
        snake = Snake((5, 5), RIGHT)
        direction = ai.decide(snake, food_cells=[(8, 5)])
        self.assertEqual(direction, (1, 0))

    def test_safe_after_rejects_a_path_that_would_seal_off_the_tail(self):
        # 3x2 grid, body walls off a 2-cell pocket with only one entrance
        #   (0,0) (1,0)=head (2,0)=tail
        #   (0,1) (1,1)=body (2,1)=body
        ai = SnakeAI(cols=3, rows=2)
        body = [(1, 0), (1, 1), (2, 1), (2, 0)]
        path_into_the_pocket = [(1, 0), (0, 0)]
        self.assertFalse(ai._safe_after(body, path_into_the_pocket))

    def test_decide_avoids_the_trap_and_chases_its_tail_instead(self):
        ai = SnakeAI(cols=3, rows=2)
        snake = Snake((1, 0), RIGHT)
        snake.body.clear()
        snake.body.extend([(1, 0), (1, 1), (2, 1), (2, 0)])

        direction = ai.decide(snake, food_cells=[(0, 0)])

        self.assertNotEqual(direction, (-1, 0))  # would walk into the dead end
        self.assertEqual(direction, (1, 0))  # chases the tail instead

    def test_hamiltonian_mode_always_returns_a_legal_next_step(self):
        ai = SnakeAI(cols=6, rows=4)
        snake = Snake((0, 0), RIGHT)
        direction = ai.decide(snake, food_cells=[(3, 2)], use_hamiltonian=True)
        self.assertIsNotNone(direction)


if __name__ == '__main__':
    unittest.main()
