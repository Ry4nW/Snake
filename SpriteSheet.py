import pygame

class SpriteSheet:
    def __init__(self, filename) -> None:
        try:
            self.sheet = pygame.image.load(filename).convert()
        except pygame.error as error:
            print(f'An error occurred while loading the spritesheet "{filename}".')
            raise SystemExit(error)
    
    def image_at(self, rectangle, colorkey = None):
        rect = pygame.Rect(rectangle)
        image = pygame.Surface(rect.size).convert()
        image.blit(self.sheet, (0, 0), rect)

        if colorkey is not None:
            if colorkey == -1:
                colorkey = image.get_at((0, 0))
            image.set_colorkey(colorkey, pygame.RLEACCEL)
        
        return image
    
    def images_at(self, rects, colorkey = None):
        return [self.image_at(rect, colorkey) for rect in rects]
            