import unittest
from collections import deque

from snake_game.snake import DOWN, LEFT, RIGHT, UP, Snake


class SnakeTests(unittest.TestCase):
    def test_advances_in_current_direction(self):
        snake = Snake((5, 5), RIGHT)
        snake.advance()
        self.assertEqual(snake.head, (6, 5))
        self.assertEqual(snake.length, 1)

    def test_grow_delays_tail_pop_by_one_tick(self):
        snake = Snake((5, 5), RIGHT)
        snake.grow()
        snake.advance()
        self.assertEqual(snake.length, 2)
        self.assertIn((5, 5), snake.body)

    def test_cannot_reverse_directly_into_its_own_neck(self):
        snake = Snake((5, 5), RIGHT)
        snake.grow()
        snake.advance()  # now length 2, reversing would hit its body
        snake.set_direction(LEFT)
        self.assertEqual(snake.direction, RIGHT)

    def test_single_segment_snake_can_reverse(self):
        snake = Snake((5, 5), RIGHT)
        snake.set_direction(LEFT)
        snake.advance()
        self.assertEqual(snake.head, (4, 5))

    def test_hits_wall_outside_grid(self):
        snake = Snake((0, 0), LEFT)
        snake.advance()
        self.assertTrue(snake.hits_wall(cols=10, rows=10))

    def test_does_not_hit_wall_inside_grid(self):
        snake = Snake((5, 5), RIGHT)
        snake.advance()
        self.assertFalse(snake.hits_wall(cols=10, rows=10))

    def test_hits_self_when_head_overlaps_body(self):
        snake = Snake((5, 5), RIGHT)
        snake.body = deque([(5, 5), (6, 5), (6, 6), (5, 6), (5, 5)])
        self.assertTrue(snake.hits_self())

    def test_does_not_hit_self_on_a_clear_body(self):
        snake = Snake((5, 5), RIGHT)
        snake.body = deque([(5, 5), (4, 5), (3, 5)])
        self.assertFalse(snake.hits_self())


if __name__ == '__main__':
    unittest.main()
