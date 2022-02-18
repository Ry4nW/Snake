"""wraps pygame_menu; settings live on GameSettings, not module globals like the old code"""
from dataclasses import dataclass

import pygame_menu

from . import config


@dataclass
class GameSettings:
    apple_count: int = config.FEW_APPLES
    ai_enabled: bool = False


class MenuController:
    def __init__(self, surface, settings: GameSettings):
        self.surface = surface
        self.settings = settings
        self._theme = pygame_menu.themes.Theme(
            title_font=pygame_menu.font.FONT_8BIT,
            background_color=(255, 0, 0, 0),
            widget_font=pygame_menu.font.FONT_8BIT,
        )
        self._start_menu = self._build_start_menu()

    def _build_start_menu(self):
        menu = pygame_menu.menu.Menu(height=300, width=400, theme=self._theme, title='Snake')
        menu.add.selector(
            'Apples: ', [('Little', config.FEW_APPLES), ('Many', config.MANY_APPLES)],
            onchange=lambda _, value: setattr(self.settings, 'apple_count', value),
        )
        menu.add.selector(
            'Player: ', [('Human', False), ('AI', True)],
            default=1 if self.settings.ai_enabled else 0,
            onchange=lambda _, value: setattr(self.settings, 'ai_enabled', value),
        )
        menu.add.button('Play', menu.disable)
        menu.add.button('Quit', pygame_menu.events.EXIT)
        return menu

    def show_start(self):
        self._start_menu.enable()
        self._start_menu.mainloop(self.surface)

    def show_game_over(self, score, high_score):
        menu = pygame_menu.menu.Menu(height=300, width=400, theme=self._theme, title='Game Over')
        menu.add.label(f'Score: {score}')
        menu.add.label(f'Best: {high_score}')
        menu.add.button('Play Again', menu.disable)
        menu.add.button('Quit', pygame_menu.events.EXIT)
        menu.mainloop(self.surface)
