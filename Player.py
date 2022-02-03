import pygame

class Player(pygame.sprite.Sprite):
	def __init__(self, image:pygame.Surface, SURFACE:pygame.Surface, next = None) -> None:
		super().__init__()    
		self.length = 1
		self.most_length = 1
		self.started = False
		self.direction = None
		self.next_direction = None
		self.screen_x = 900
		self.screen_y = 600
		self.x = 200
		self.y = 300
		self.image = image
		self.SURFACE = SURFACE
		self.image.set_colorkey((255, 255, 255))
		self.rect = self.image.get_rect()
		self.next = next

	def move(self) -> bool:
		if self.direction == 'left' and (50 <= self.x - 1): self.x -= 1
		elif self.direction == 'right' and (self.x + 1 <= self.screen_x): self.x += 1
		elif self.direction == 'up' and (49 <= self.y - 1): self.y -= 1
		elif self.direction == 'down' and (self.y + 1 <= self.screen_y): self.y += 1
		else: return False
		return True

	def draw(self, player_fat) -> None:
		for snake in player_fat:
			self.SURFACE.blit(self.image, (snake[0], snake[1]))	

	def get_rect(self) -> pygame.Rect:
		return pygame.Rect(self.x, self.y, 50, 50)
