# Snake (Python)

> This is the original Python/pygame implementation. The primary, actively
> developed version is now the [C++/raylib rewrite](../cpp/README.md) in
> `../cpp/` — see the top-level [README](../README.md) for how the two
> relate. This one is kept as-is for reference.

A grid-based Snake game in Python and Pygame, with sprite-sheet animation,
audio, menus, persistent high scores, and an AI autopilot that plays the
game using pathfinding and reachability analysis instead of a fixed
strategy.

## Features

- Sprite-sheet-driven rendering (single PNG, sliced into apple / body /
  four directional head sprites) instead of individual image files.
- Menu-driven setup (apple count, Human vs. AI player) and a game-over
  screen, both via `pygame_menu`.
- Sound effects for eating and death, score display, and a persistent
  all-time high score (`highscore.json`) that survives restarts.
- An AI autopilot (toggle with `Tab`) that plays the game itself using
  A* search, a reachability safety check, tail-chasing, and an optional
  Hamiltonian-cycle mode. See [Technical Decisions](#technical-decisions).

## Controls

| Action | Keys |
|---|---|
| Move | Arrow keys or `WASD` |
| Toggle AI autopilot | `Tab` |
| Toggle AI strategy (safe A* vs. Hamiltonian cycle) | `H` |
| Quit | Close the window |

Run with `python main.py --ai` to start a round with the AI already
driving.

## How to Run

```
pip install -r requirements.txt
python main.py
```

Run the test suite (pure game logic only — no display required):

```
python -m unittest discover -s tests
```

## Architecture

The original version of this project was a single ~200-line loop in
`main.py` that mixed input handling, movement, rendering, collision
detection, apple spawning, and menu state together, with the snake's
position tracked as loose module-level variables. It worked, but there
was no separation between "what is true about the game" and "how it gets
drawn," which made the AI feature below hard to add and impossible to
unit test.

It's now organized as a small package, `snake_game/`, split along that
exact seam — logic that's pure grid math with no `pygame` import, versus
everything that touches a `Surface`:

```
main.py                    entry point: parses --ai, builds and runs Game

snake_game/
  config.py                 grid size, timing, colors — no magic numbers elsewhere
  snake.py                   Snake: body, direction, growth, collision rules
  food.py                     FoodManager: spawn/consume, avoids occupied cells
  highscore.py                 HighScoreStore: JSON persistence
  game.py                       Game: state machine + fixed-timestep loop, owns everything below
  renderer.py                    Renderer: draws board/snake/food/HUD, no game logic
  audio_manager.py                 AudioManager: sound effects, fails soft with no audio device
  input_handler.py                  InputHandler: pygame events -> game commands
  menu.py                            MenuController + GameSettings: start/game-over menus
  assets.py                          one-time sprite/trophy loading
  sprite_sheet.py                    SpriteSheet: slices a sprite sheet by rectangle
  sprite_loader.py                    SpriteLoader: names the rectangles for this sheet
  ai/
    pathfinding.py                    generic grid BFS / A* / flood fill
    hamiltonian.py                     builds a Hamiltonian cycle over the grid
    snake_ai.py                        SnakeAI: the decision strategy, see below

tests/                        unit tests for everything under snake_game/ that
                               doesn't touch pygame (snake, food, highscore, ai/*)
```

`Snake`, `FoodManager`, `HighScoreStore`, and the entire `ai/` package have
no `pygame` import at all — they operate purely on `(col, row)` grid
tuples. That's what makes them unit-testable without a display, and it's
also what let the AI be built and tested as ordinary graph algorithms
before it ever touched a `Snake` object.

**Game loop.** `Game._play_round` polls input and renders every frame
(`RENDER_FPS = 60`), but only steps the snake `LOGIC_TICKS_PER_SECOND`
(7) times a second, via a time accumulator. That's a standard
fixed-timestep pattern: input latency and frame rate stay decoupled from
how fast the snake moves, so either one can change independently.

## Technical Decisions

### Why a sprite sheet

Loading one PNG and slicing it by rectangle (`SpriteSheet.image_at`)
avoids shipping a dozen loose image files and is how 2D sprite assets are
typically packaged. `SpriteLoader` is the only place that knows the pixel
rectangles for this specific sheet; `SpriteSheet` itself is generic.

### Why grid coordinates instead of pixels

`Snake` and `FoodManager` operate on `(col, row)` integers, not screen
pixels. `Renderer` is the only place that converts a cell to a pixel
(`config.cell_to_pixel`). This is also what makes the grid a plain graph
that `ai/pathfinding.py` can search — the AI and the renderer share the
exact same coordinate space the game logic already uses, instead of a
separate one built for search.

### The AI: why A* alone isn't good enough

The obvious approach is: every tick, run A* from the snake's head to the
nearest apple, and follow it. This works right up until it doesn't —
A* only optimizes for *shortest path to food*, not *survives afterward*.
A snake can easily A\*-its-way into a pocket that's the shortest route to
an apple but has no exit once its own body seals the entrance behind it.

`SnakeAI.decide` (`snake_game/ai/snake_ai.py`) layers four strategies,
falling back in order:

1. **Safe A\*.** Find the A* path to the nearest food. Before committing,
   simulate walking that entire path — including the length increase
   from eating — and run a second BFS from the simulated head to the
   simulated tail. If the tail is still reachable, the rest of the board
   is provably still open, so the path is safe. If not, it's rejected
   even though it's the shortest one (`_safe_after`).
2. **Tail chasing.** If no food path is safe, path toward the snake's own
   tail instead. The tail is always reachable — it's a cell that's about
   to be vacated — so following it buys time without shrinking the
   region the snake can still reach.
3. **Max-space survival.** If even that fails, take whichever legal move
   leaves the largest reachable area, via flood fill from each candidate
   next cell. This is the last resort when the board is nearly full.
4. **Hamiltonian cycle (optional, `H` to toggle).** Ignore food entirely
   and follow a precomputed cycle that visits every cell exactly once
   and returns to the start. A snake following it can never trap itself,
   because the "safe region" is the entire board by construction — the
   tradeoff is that it's a much slower way to actually fill the board.
   This only exists for grids with an even cell count (`GRID_COLS *
   GRID_ROWS` is bipartite by `(row + col)` parity, and a Hamiltonian
   cycle needs equal counts of each color).

`tests/test_snake_ai.py` includes a hand-built 3x2 grid where the direct
A* path walks into a dead end; it asserts `_safe_after` rejects it and
that `decide` falls back to chasing the tail instead of taking the trap.

### Growth timing

Eating is resolved by checking whether the *next* head cell (before
`advance()` is called) is food, and calling `snake.grow()` first if so.
That makes the length increase apply on the same tick as the eat, rather
than lagging a tick behind — `Snake.advance()` only pops the tail when
there's no pending growth, so calling `grow()` before `advance()` skips
that pop immediately.

### One-time asset loading

The trophy icon used to be reloaded from disk (`pygame.image.load`)
every single frame. `snake_game/assets.py` now loads every sprite once
at startup; `Renderer` only blits already-loaded surfaces.

## Future Work

- Movement interpolation (animate between cells instead of snapping),
  now that rendering and logic ticks are already decoupled.
- A hybrid AI mode that takes A* shortcuts across a Hamiltonian cycle
  when they're provably safe, rather than treating the two as separate
  modes.
- Additional food types / power-ups (deliberately out of scope for this
  pass — the priority was engineering structure and the AI, not content).

## Credits

Snake spritesheet: https://opengameart.org/content/snake-sprite-sheet
