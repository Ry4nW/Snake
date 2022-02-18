from .sprite_sheet import SpriteSheet


class SpriteLoader:
    """where each named sprite lives on the sheet"""

    def __init__(self, spritesheet: SpriteSheet) -> None:
        self.apple = spritesheet.image_at((0, 192, 64, 64))

        self.head_left = spritesheet.image_at((192, 64, 64, 64))
        self.head_right = spritesheet.image_at((256, 0, 64, 64))
        self.head_up = spritesheet.image_at((192, 0, 64, 64))
        self.head_down = spritesheet.image_at((256, 64, 64, 64))
        self.body = spritesheet.image_at((64, 0, 64, 64))
