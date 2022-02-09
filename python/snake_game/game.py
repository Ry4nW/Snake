"""game state machine + fixed-timestep loop. renders every frame, steps snake via accumulator"""
import sys

from . import config
from .assets import load_sprites
from .audio_manager import AudioManager
from .food import FoodManager
from .highscore import HighScoreStore
from .input_handler import InputHandler
from .menu import GameSettings, MenuController
from .renderer import Renderer
from .snake import RIGHT, Snake
from .ai.snake_ai import SnakeAI

START_CELL = (3, 5)


class Game:
    def __init__(self, start_with_ai=False):
        import pygame  # deferred, so import doesn't need a display
        self._pygame = pygame

        pygame.init()
        pygame.display.set_caption('Snake')
        pygame.display.set_icon(pygame.image.load('assets/snake_game_icon.jpg'))

        self.surface = pygame.display.set_mode((config.WINDOW_WIDTH, config.WINDOW_HEIGHT))
        self.font = pygame.font.SysFont('Arial', 30)
        self.clock = pygame.time.Clock()

        self.sprites = load_sprites()
        self.audio = AudioManager()
        self.high_score = HighScoreStore(config.HIGH_SCORE_FILE)
        self.input = InputHandler()
        self.renderer = Renderer(self.surface, self.sprites, self.font)
        self.ai = SnakeAI(config.GRID_COLS, config.GRID_ROWS)

        self.settings = GameSettings(ai_enabled=start_with_ai)
        self.menu = MenuController(self.surface, self.settings)

        self.last_score = 0

    def run(self):
        try:
            self.menu.show_start()
            while True:
                self._play_round()
                self.menu.show_game_over(self.last_score, self.high_score.value)
        finally:
            self._pygame.quit()

    def _play_round(self):
        pygame = self._pygame
        snake = Snake(START_CELL, RIGHT)
        food = FoodManager(config.GRID_COLS, config.GRID_ROWS, count=self.settings.apple_count)
        food.reset(snake.occupied_cells())

        ai_active = self.settings.ai_enabled
        use_hamiltonian = False
        started = ai_active  # human needs a keypress first, AI doesn't wait

        accumulator = 0.0
        step_seconds = 1.0 / config.LOGIC_TICKS_PER_SECOND

        while True:
            frame_ms = self.clock.tick(config.RENDER_FPS)
            accumulator += frame_ms / 1000.0

            for command, value in self.input.poll():
                if command == 'quit':
                    pygame.quit()
                    sys.exit(0)
                elif command == 'toggle_ai':
                    ai_active = not ai_active
                    started = True
                elif command == 'toggle_ai_mode':
                    use_hamiltonian = not use_hamiltonian
                elif command == 'direction' and not ai_active:
                    snake.set_direction(value)
                    started = True

            while accumulator >= step_seconds:
                accumulator -= step_seconds
                if not started:
                    continue

                if ai_active:
                    direction = self.ai.decide(snake, food.items, use_hamiltonian)
                    if direction:
                        snake.set_direction(direction)

                next_head = snake.peek_next_head()
                eating = next_head in food.items
                if eating:
                    snake.grow()
                snake.advance()

                if snake.hits_wall(config.GRID_COLS, config.GRID_ROWS) or snake.hits_self():
                    self.audio.play_death()
                    self.last_score = snake.length - 1
                    self.high_score.update(self.last_score)
                    return

                if eating:
                    self.audio.play_eat()
                    food.consume(next_head, snake.occupied_cells())

            self._render(snake, food, ai_active)

    def _render(self, snake, food, ai_active):
        self.renderer.draw_board()
        self.renderer.draw_food(food.items)
        self.renderer.draw_snake(snake)
        self.renderer.draw_hud(snake.length - 1, self.high_score.value, ai_active)
        self._pygame.display.flip()
