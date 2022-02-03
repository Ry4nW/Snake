from SpriteSheet import SpriteSheet

class LoadSprites:
    def __init__(self, spritesheet:SpriteSheet) -> None:
        self.spritesheet = spritesheet

        self.apple = self.spritesheet.image_at((0, 192, 64, 64))

        self.head_left = self.spritesheet.image_at((192, 64, 64, 64))
        self.head_right = self.spritesheet.image_at((256, 0, 64, 64))
        self.head_up = self.spritesheet.image_at((192, 0, 64, 64))
        self.head_down = self.spritesheet.image_at((256, 64, 64, 64))
        self.body = self.spritesheet.image_at((64, 0, 64, 64))
