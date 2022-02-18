"""maps pygame key events to game commands"""
import pygame

from .snake import DOWN, LEFT, RIGHT, UP

DIRECTION_KEYS = {
    pygame.K_LEFT: LEFT, pygame.K_a: LEFT,
    pygame.K_RIGHT: RIGHT, pygame.K_d: RIGHT,
    pygame.K_UP: UP, pygame.K_w: UP,
    pygame.K_DOWN: DOWN, pygame.K_s: DOWN,
}


class InputHandler:
    def poll(self):
        commands = []
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                commands.append(('quit', None))
            elif event.type == pygame.KEYDOWN:
                if event.key in DIRECTION_KEYS:
                    commands.append(('direction', DIRECTION_KEYS[event.key]))
                elif event.key == pygame.K_TAB:
                    commands.append(('toggle_ai', None))
                elif event.key == pygame.K_h:
                    commands.append(('toggle_ai_mode', None))
        return commands
