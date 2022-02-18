"""all drawing lives here, no game state decisions. safe to call every frame."""
import pygame

from . import config
from .snake import DOWN, LEFT, RIGHT, UP

HEAD_SPRITE_BY_DIRECTION = {
    UP: 'head_up',
    DOWN: 'head_down',
    LEFT: 'head_left',
    RIGHT: 'head_right',
}


class Renderer:
    def __init__(self, surface, sprites, font):
        self.surface = surface
        self.sprites = sprites
        self.font = font

    def draw_board(self):
        self.surface.fill(config.COLORS['BACKGROUND_GREEN'])
        pygame.draw.rect(
            self.surface,
            config.COLORS['BOARD_YELLOW_GREEN'],
            (config.BOARD_MARGIN, config.BOARD_MARGIN, config.BOARD_WIDTH, config.BOARD_HEIGHT),
        )
        for row in range(config.GRID_ROWS):
            for col in range(config.GRID_COLS):
                if (row + col) % 2 == 0:
                    continue
                x, y = config.cell_to_pixel((col, row))
                pygame.draw.rect(
                    self.surface, config.COLORS['BOARD_DARK_GREEN'],
                    (x, y, config.CELL_SIZE, config.CELL_SIZE),
                )

    def draw_food(self, food_items):
        for cell in food_items:
            self.surface.blit(self.sprites['apple'], config.cell_to_pixel(cell))

    def draw_snake(self, snake):
        head_sprite = self.sprites[HEAD_SPRITE_BY_DIRECTION[snake.direction]]
        for i, cell in enumerate(snake.body):
            sprite = head_sprite if i == 0 else self.sprites['body']
            self.surface.blit(sprite, config.cell_to_pixel(cell))

    def draw_hud(self, score, high_score, ai_active):
        white = (255, 255, 255)
        self.surface.blit(self.font.render(str(score), True, white), (45, 10))
        self.surface.blit(pygame.transform.scale(self.sprites['apple'], (30, 30)), (75, 10))
        self.surface.blit(self.sprites['trophy'], (150, 10))
        self.surface.blit(self.font.render(str(high_score), True, white), (185, 10))
        if ai_active:
            self.surface.blit(self.font.render('AI', True, (255, 215, 0)), (900, 10))
