"""tunable constants: grid size, timing, colors. no logic here."""
import pygame

CELL_SIZE = 50
GRID_COLS = 18
GRID_ROWS = 12
BOARD_MARGIN = 50

BOARD_WIDTH = GRID_COLS * CELL_SIZE
BOARD_HEIGHT = GRID_ROWS * CELL_SIZE
WINDOW_WIDTH = BOARD_WIDTH + 2 * BOARD_MARGIN
WINDOW_HEIGHT = BOARD_HEIGHT + 2 * BOARD_MARGIN

# renders every frame at RENDER_FPS, steps snake at LOGIC_TICKS_PER_SECOND
RENDER_FPS = 60
LOGIC_TICKS_PER_SECOND = 7

FEW_APPLES = 1
MANY_APPLES = 3

HIGH_SCORE_FILE = 'highscore.json'

COLORS = {
    'BLACK': pygame.Color(0, 0, 0),
    'WHITE': pygame.Color(255, 255, 255),
    'BACKGROUND_GREEN': pygame.Color(95, 161, 85),
    'BOARD_YELLOW_GREEN': pygame.Color(170, 227, 104),
    'BOARD_DARK_GREEN': pygame.Color(103, 145, 54),
}


def cell_to_pixel(cell):
    col, row = cell
    return (BOARD_MARGIN + col * CELL_SIZE, BOARD_MARGIN + row * CELL_SIZE)
