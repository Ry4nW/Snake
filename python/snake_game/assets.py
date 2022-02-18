"""load assets once, up front. old code reloaded trophy.png every frame."""
import pygame

from . import config
from .sprite_loader import SpriteLoader
from .sprite_sheet import SpriteSheet


def load_sprites():
    spritesheet = SpriteSheet('assets/snake_spritesheet.png')
    raw = SpriteLoader(spritesheet)

    sprites = {}
    for name, surface in vars(raw).items():
        surface.set_colorkey(config.COLORS['WHITE'])
        sprites[name] = pygame.transform.scale(surface, (config.CELL_SIZE, config.CELL_SIZE))

    trophy = pygame.image.load('assets/trophy.png').convert()
    trophy.set_colorkey(config.COLORS['BLACK'])
    sprites['trophy'] = pygame.transform.scale(trophy, (30, 30))

    return sprites
