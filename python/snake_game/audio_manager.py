import pygame


class AudioManager:
    def __init__(self, asset_dir='assets'):
        self.enabled = True
        try:
            self._eat = pygame.mixer.Sound(f'{asset_dir}/eating sound effect.wav')
            self._death = pygame.mixer.Sound(f'{asset_dir}/death sound.wav')
        except pygame.error:
            # no audio device (headless/CI), fail soft
            self.enabled = False
            self._eat = None
            self._death = None

    def play_eat(self):
        if self.enabled:
            self._eat.play()

    def play_death(self):
        if self.enabled:
            self._death.play()
