# Snake (C++)

C++ rewrite of the python version, using raylib for rendering. Same idea: an AI autopilot that plays the game using pathfinding instead of a fixed strategy.

## Features

- Sprite sheet rendering - one texture, scaled on the GPU per draw call instead of pre-cropped images
- Simple keyboard-only menu for setup and the game over screen
- Sound effects, score, and a persistent high score in `highscore.json`
- AI autopilot (`Tab`) - A* with a safety check, tail chasing, flood fill survival, and an optional Hamiltonian cycle mode
- Debug overlay (`V`) showing what the AI actually looked at on its last move
- Movement interpolation so it doesn't look like it's snapping between cells
- Replay recording and playback (`--record` / `--replay`)
- Core game logic (`snake_core`) has zero raylib dependency, so tests run without a window or GPU

## Controls

| Action | Keys |
|---|---|
| Move | Arrow keys or `WASD` |
| Toggle AI autopilot | `Tab` |
| Toggle AI strategy (safe A* vs. Hamiltonian) | `H` |
| Toggle debug overlay | `V` |
| Menu navigation | Arrows/`WASD`, `Enter` or `Space` to select |
| Quit | `Esc` or close the window |

```
./build/snake_cpp --record run.json   # play normally, first round gets saved to run.json
./build/snake_cpp --replay run.json   # watch that round play back exactly
```

## Build and run

Needs CMake 3.16+ and a C++20 compiler. raylib gets pulled in automatically via `FetchContent`. On Linux you'll need the usual GL/X11 dev headers (`libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev`) which most desktop setups already have. Windows/macOS should just work.

```
cmake -B build -S .
cmake --build build -j
./build/snake_cpp            # play
./build/snake_cpp --ai       # start with AI driving
```

Tests, no window or GPU needed:

```
./build/snake_tests
```

## Layout

```
cpp/
  CMakeLists.txt        snake_core (lib), snake_cpp (exe), snake_tests (exe)
  third_party/           vendored doctest.h and json.hpp
  src/
    cell.hpp, direction.hpp, config.hpp    basic grid types, no raylib
    snake.hpp/.cpp        body, growth, collision
    food.hpp/.cpp          spawning
    highscore.hpp/.cpp      json persistence
    replay.hpp/.cpp          records/replays a round
    ai/
      pathfinding.hpp/.cpp   generic bfs / a* / flood fill, nothing snake-specific
      hamiltonian.hpp/.cpp    builds the hamiltonian cycle
      snake_ai.hpp/.cpp        the actual decision logic
    renderer, audio_manager, input_handler, menu, game, main     raylib side
  tests/    one test file per snake_core module
  assets/   sprites, sounds, icons
```

`snake_core` is everything under `ai/` plus snake/food/highscore/replay - none of it includes `raylib.h`, so `snake_tests` links against just that and runs without a display. Only `snake_cpp`, the actual game, touches raylib.

## Why A* alone doesn't work

Running A* to the nearest apple every tick sounds fine but it can walk the snake straight into a dead end, since A* only cares about the shortest path, not whether there's a way out afterward.

So `SnakeAI::Decide` tries a few things in order:

1. A* to the food, but only if a second BFS confirms the snake can still reach its own tail after taking that path. If it can't, the path gets rejected even though it's the shortest one.
2. If no food path is safe, chase the tail instead. Always reachable, since it's a cell about to be vacated.
3. If even that's blocked, take whatever legal move leaves the most open space (flood fill).
4. Optional: follow a precomputed Hamiltonian cycle that visits every cell once. Can never trap itself, but ignores food and fills the board slowly. Only works when `cols * rows` is even.

`test_snake_ai.cpp` hand-builds a grid where the direct path to food is a trap, to check the safety check actually catches it.

## A few other notes

- Growth timing: the next head cell gets checked for food before `Advance()` runs, so eating and growing land on the same tick instead of a tick behind.
- `Game`'s members are ordered so `WindowContext` (which calls `InitWindow`) gets constructed before anything needing a GL context, like `SpriteSheet`. C++ builds members in declaration order no matter what order they're listed in the constructor.
- Replay just stores the starting conditions plus every direction the snake actually committed to each tick, not raw input or frame timing, so playback is exact.

## Future ideas

- AI that takes A* shortcuts across the Hamiltonian cycle when it's provably safe
- Mouse support in the menu
- Variable speed / frame stepping through a replay

## Credits

Snake spritesheet: https://opengameart.org/content/snake-sprite-sheet
