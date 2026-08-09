# Snake (Python)

Old python/pygame version. Not really maintained anymore, the [C++ rewrite](../cpp/README.md) in `../cpp/` is the one I actually work on now. Keeping this around for reference.

Grid based snake with sprite animation, sound, menus, high scores, and an AI that plays itself instead of using a fixed strategy.

## Features

- Sprite sheet rendering instead of a bunch of separate image files
- Menu for setup (apple count, human vs AI) and game over, via `pygame_menu`
- Sound effects, score, and a persistent high score (`highscore.json`)
- AI autopilot (`Tab`): A* with a safety check, tail chasing, and an optional Hamiltonian cycle mode

## Controls

| Action | Keys |
|---|---|
| Move | Arrow keys or `WASD` |
| Toggle AI autopilot | `Tab` |
| Toggle AI strategy (safe A* vs. Hamiltonian) | `H` |
| Quit | Close the window |

Run with `python main.py --ai` to start with the AI already driving.

## Run it

```
pip install -r requirements.txt
python main.py
```

Tests, no display needed:

```
python -m unittest discover -s tests
```

## Layout

This used to be one ~200 line `main.py` with everything mixed together - input, movement, rendering, collision, all tracked in loose module-level variables. It's now split into `snake_game/`, separating stuff with no pygame import (pure game logic) from stuff that touches a `Surface`.

```
main.py

snake_game/
  config.py       grid size, timing, colors
  snake.py         body, direction, growth, collision
  food.py           spawning
  highscore.py       json persistence
  game.py             state machine, fixed timestep loop
  renderer.py          drawing, no game logic
  audio_manager.py      sound effects
  input_handler.py       pygame events -> game commands
  menu.py                 start/game-over menus
  assets.py, sprite_sheet.py, sprite_loader.py   sprite loading
  ai/
    pathfinding.py    bfs / a* / flood fill
    hamiltonian.py     hamiltonian cycle
    snake_ai.py          decision logic

tests/    covers everything that doesn't touch pygame
```

`Snake`, `FoodManager`, `HighScoreStore`, and everything in `ai/` never import pygame - they just work on `(col, row)` tuples. That's what makes them testable without a display.

The game loop renders every frame but only steps the snake 7 times a second, using a time accumulator, so movement speed and frame rate stay independent.

## Why A* alone doesn't work

Running A* to the nearest apple every tick sounds fine but it can walk the snake into a dead end, since A* only cares about the shortest path, not whether there's a way out afterward.

So `SnakeAI.decide` tries a few things in order:

1. A* to the food, but only if a second BFS confirms the tail is still reachable afterward (`_safe_after`). If not, the path gets rejected even if it's the shortest one.
2. If no food path is safe, chase the tail instead, since that's always reachable.
3. If that's not possible either, take whatever move leaves the most open space via flood fill.
4. Optional: a precomputed Hamiltonian cycle that visits every cell once. Can't trap itself, ignores food, slow. Only works if the grid has an even number of cells.

`tests/test_snake_ai.py` hand-builds a grid where the direct path is a trap, to check the safety check actually rejects it.

## Other notes

- Sprite sheet: one PNG sliced by rectangle instead of separate image files.
- Grid coords instead of pixels: `Snake` and `FoodManager` just use `(col, row)` ints, `Renderer` is the only thing that converts to pixels.
- Growth: the next head cell gets checked for food before `advance()` runs, so eating and growing happen on the same tick.
- Assets load once at startup now instead of the trophy icon getting reloaded from disk every frame like before.

## Future ideas

- Movement interpolation between cells
- AI that takes A* shortcuts across the Hamiltonian cycle when safe
- More food types / power ups

## Credits

Snake spritesheet: https://opengameart.org/content/snake-sprite-sheet
