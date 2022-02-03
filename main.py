import pygame
import pygame_menu
from pygame.locals import *

from LoadSprites import LoadSprites
from Player import Player
from SpriteSheet import SpriteSheet

import sys
import random

balls_amount = None

def set_balls_amount(value, amount):
    global balls_amount
    balls_amount = amount

def menu(menu_object, surface:pygame.surface, start):
	def disable():
		menu_object.disable()
	menu_object.enable()
	if start:
		menu_object.add.selector('Balls: ', [('Little', 1), ('Many', 2)], onchange = set_balls_amount)
		menu_object.add.button('Play', disable)
		menu_object.add.button('Quit', pygame_menu.events.EXIT)
	menu_object.mainloop(surface)

def main():
	pygame.init()

	# Declaring basic elements.
	pygame.display.set_caption('Snake')
	pygame.display.set_icon(pygame.image.load('assets/snake_game_icon.jpg'))

	FPS = 7
	FPS_CLOCK = pygame.time.Clock()

	DIMENSION_X = 1000
	DIMENSION_Y = 700
	SURFACE = pygame.display.set_mode((DIMENSION_X, DIMENSION_Y))
	FONT = pygame.font.SysFont('Arial', 30)

	COLORS = {
		'BLACK' : pygame.Color(0, 0, 0),
		'WHITE' : pygame.Color(255, 255, 255),
		'BACKGROUND_GREEN' : pygame.Color(95, 161, 85),
		'BOARD_YELLOW_GREEN' : pygame.Color(170, 227, 104),
		'BOARD_DARK_GREEN' : pygame.Color(103, 145, 54)
	}

	eating_effect = pygame.mixer.Sound('assets/eating sound effect.wav')
	death_effect = pygame.mixer.Sound('assets/death sound.wav')

	# Creating and loading in the first instance of the menu.
	menu_theme = pygame_menu.themes.Theme(
		title_font = pygame_menu.font.FONT_8BIT,
		background_color = (255, 0, 0, 0),
		widget_font = pygame_menu.font.FONT_8BIT
	)

	menu_object = pygame_menu.menu.Menu(
		height = 300,
		width = 400,
		theme = menu_theme,
		title = 'Snake',
	)

	menu(menu_object, SURFACE, True)

	# Loading in the spritesheet and getting all attributes (sprites) of the LoadSprites instance.
	spritesheet = SpriteSheet('assets/snake_spritesheet.png')
	load_sprites = LoadSprites(spritesheet)
	all_sprites: dict[str:pygame.Surface] = vars(load_sprites)

	# Setting the colorkey of all the sprites so they can achieve transparency.
	for sprite in all_sprites:
		if sprite != 'spritesheet':
			all_sprites[sprite].set_colorkey(COLORS['WHITE'])
			all_sprites[sprite] = pygame.transform.scale(all_sprites[sprite], (50, 50))

	# Initializing the player.
	player = Player(all_sprites['body'], SURFACE)

	# Variables for apple coordiantes.

	x_coords = []
	y_coords = []

	for i in range(50, 901, 50):
		x_coords.append(i)
	for i in range(50, 601, 50):
		y_coords.append(i)
	
	if balls_amount == 2: apples = [[700, 300], [400, 200], [100, 500]]
	else: apples = [[700, 300]]
	print(apples)
	# Storing all fat blocks of the player.
	player_fat = []

	# Reset the game when player loses.
	def new_game(player):
		new_player = Player(all_sprites['body'], SURFACE)
		new_player.most_length = player.most_length
		player = new_player
		menu(menu_object, SURFACE, False)
		return player

	# -------- Main program loop. -------- 
	while True:
		pygame.display.update()
		events = pygame.event.get()

		for event in events:
			if event.type == pygame.QUIT:
				pygame.quit()
				sys.exit(0)
			
			# Checking for user input on the movement of the snake.
			if event.type == pygame.KEYDOWN:
				if not player.started:
					player.started = True
				if (event.key == pygame.K_LEFT or event.key == pygame.K_a):
					x_change = -50
					y_change = 0
				if (event.key == pygame.K_RIGHT or event.key == pygame.K_d):
					x_change = 50
					y_change = 0
				if (event.key == pygame.K_UP or event.key == pygame.K_w):
					x_change = 0
					y_change = -50
				if (event.key == pygame.K_DOWN or event.key == pygame.K_s):
					x_change = 0
					y_change = 50

		# Drawing in the background and the board.
		SURFACE.fill(COLORS['BACKGROUND_GREEN'])
		pygame.draw.rect(SURFACE, COLORS['BOARD_YELLOW_GREEN'], (50, 50, 900, 600))

		cur_row = 0

		for i in range(50, 601, 50):
			if cur_row % 2 == 0:
				for j in range(100, 1001, 100):
					pygame.draw.rect(SURFACE, COLORS['BOARD_DARK_GREEN'], (j, i, 50, 50))
			else:
				for j in range(50, 901, 100):
					pygame.draw.rect(SURFACE, COLORS['BOARD_DARK_GREEN'], (j, i, 50, 50))

			cur_row += 1

        # The apples and the scores.
		SURFACE.blit(FONT.render(str(player.length - 1), True, (255, 255, 255)), (45, 10))
		all_sprites['apple'].set_colorkey(COLORS['WHITE'])

		for ball in apples:
			SURFACE.blit(all_sprites['apple'], (ball[0], ball[1]))
			
		SURFACE.blit(pygame.transform.scale(all_sprites['apple'], (30, 30)), (75, 10))
		trophy_icon: pygame.Surface = pygame.image.load('assets/trophy.png').convert()
		trophy_icon.set_colorkey(COLORS['BLACK'])
		SURFACE.blit(pygame.transform.scale(trophy_icon, (30, 30)), (150, 10))
		SURFACE.blit(FONT.render(str(player.most_length - 1), True, (255, 255, 255)), (120, 10))

        # Checking for wall collisions. 
		if player.x > 950 or player.x < 50 or player.y > 650 or player.y < 50: 
			death_effect.play()
			player_fat.clear()
			player = new_game(player)
		
		if player.started:
			player.x += x_change
			player.y += y_change

		current_fat = [player.x, player.y]
		player_fat.append(current_fat)

		if len(player_fat) > player.length:
			del player_fat[0]

		# Checking for collisions of the player's head on it's own body.
		if player.started:
			for fat in player_fat[:-1]:
				if fat == current_fat: 
					death_effect.play()
					player_fat.clear()
					player = new_game(player)

		player.draw(player_fat)

		# pygame.display.update()

		# Collision detection of the snake with a ball.
		for ball in apples:
			if player.get_rect().colliderect(pygame.Rect(ball[0], ball[1], 50, 50)):
				eating_effect.play()
				player.length += 1
				# Score.
				SURFACE.blit(FONT.render(str(player.length - 1), True, (255, 255, 255)), (50, 10))
				# Checking if current score is more than the all time high score.
				if player.length > player.most_length:
					player.most_length = player.length 
					SURFACE.blit(FONT.render(str(player.most_length - 1), True, (255, 255, 255)), (125, 10))

				# New random position for ball(s).
				ball[0] = x_coords[random.randint(0, len(x_coords) - 1)]
				ball[1] = y_coords[random.randint(0, len(y_coords) - 1)]

		FPS_CLOCK.tick(FPS)
		pygame.display.flip()
		print(apples)
    
if __name__ == '__main__':
	main()
