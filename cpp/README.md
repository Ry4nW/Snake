# Snake (C++)

A grid-based Snake game in modern C++ and [raylib](https://www.raylib.com/), with an AI autopilot that plays the game using pathfinding and reachability analysis instead of a fixed strategy. This is a from-scratch C++ rewrite of an earlier [Python/pygame version](../python/README.md); see [Technical Decisions](#technical-decisions) for what actually changed versus what's just a translation.

## Features

- GPU-driven sprite-sheet rendering: one texture, sliced and scaled per draw call via `DrawTexturePro`, instead of pre-cropped CPU surfaces.
- A hand-rolled, keyboard-navigable menu (raylib ships no widget toolkit) for game setup and the game-over screen.
- Sound effects, score display, and a persistent all-time high score (`highscore.json`) via a vendored single-header JSON library.
- An AI autopilot (toggle with `Tab`) using A* search, a BFS reachability safety check, tail-chasing, flood-fill survival, and an optional Hamiltonian-cycle mode. See [Technical Decisions](#technical-decisions).
- A live debug overlay (`V`) drawing exactly what the AI considered on its last decision -- the A* path, whether it passed the safety check, and the flood-fill region backing a survival move -- instead of only the direction it settled on.
- Movement interpolation: segment positions are linearly interpolated between the last two logic ticks based on how far into the next tick the current frame is, so motion reads as smooth instead of snapping once per 1/7s tick, without touching the discrete simulation itself.
- Deterministic replay (`--record <path>` / `--replay <path>`): a round is fully reproducible from its start conditions plus the sequence of committed directions, so a run can be saved and played back exactly.
- A `snake_core` static library with **zero raylib dependency** — the entire simulation, AI, and replay serialization are unit tested via [doctest](https://github.com/doctest/doctest) without a window, a GPU, or a display.

## Controls

| Action | Keys |
|---|---|
| Move | Arrow keys or `WASD` |
| Toggle AI autopilot | `Tab` |
| Toggle AI strategy (safe A* vs. Hamiltonian cycle) | `H` |
| Toggle AI debug overlay | `V` |
| Menu navigation | Arrows / `WASD`, `Enter` or `Space` to select |
| Quit | `Esc` or close the window |

```
./build/snake_cpp --record run.json   # play normally; the first round is saved to run.json on death
./build/snake_cpp --replay run.json   # watch that exact round play back deterministically
```

## How to Build and Run

Requires CMake 3.16+ and a C++20 compiler. raylib is fetched and built automatically via `FetchContent` — nothing to install system-wide beyond the usual Linux GL/X11 dev headers (`libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev`), which most desktop Linux setups already have. On Windows/macOS, raylib's own CMake build handles the platform windowing backend without extra setup.

```
cmake -B build -S .
cmake --build build -j
./build/snake_cpp            # play
./build/snake_cpp --ai       # start with the AI already driving
```

Run the tests (pure simulation/AI logic only — no window or GPU required):

```
./build/snake_tests
```

## Architecture

```mermaid
graph TD
    subgraph snake_core [snake_core -- no raylib dependency]
        Snake
        FoodManager
        HighScoreStore
        SnakeAI --> pathfinding
        SnakeAI --> hamiltonian
        SnakeAI -.uses.-> Snake
    end

    subgraph snake_cpp [snake_cpp -- links raylib]
        main --> Game
        Game --> WindowContext
        Game --> SpriteSheet
        Game --> OwnedTexture
        Game --> AudioManager
        Game --> InputHandler
        Game --> Renderer
        Game --> MenuController
        Renderer --> SpriteSheet
        Renderer --> OwnedTexture
    end

    Game --> Snake
    Game --> FoodManager
    Game --> HighScoreStore
    Game --> SnakeAI

    subgraph snake_tests [snake_tests -- doctest, links snake_core only]
        tests[test_*.cpp]
    end
    tests --> snake_core
```

The split mirrors the Python version's core idea, translated to a build-system-enforced boundary instead of a convention: `snake_core` (Cell, Direction, Snake, FoodManager, HighScoreStore, and all of `ai/`) is a static library that never includes `<raylib.h>`, so `snake_tests` links against it directly with no window, texture, or audio device involved. Only `snake_cpp` — the actual playable executable — links raylib, and it's where `Renderer`, `AudioManager`, `InputHandler`, `MenuController`, and the RAII resource wrappers live.

```
cpp/
  CMakeLists.txt         three targets: snake_core (lib), snake_cpp (exe), snake_tests (exe)
  third_party/            vendored single-header doctest.h and json.hpp
  src/
    cell.hpp               a grid (col, row) cell + hash, no dependencies
    direction.hpp            Up/Down/Left/Right + delta/opposite helpers
    config.hpp                 grid size, timing, CellToPixel -- no raylib
    snake.hpp / .cpp             body, growth, collision -- the simulation
    food.hpp / .cpp                spawn/consume, avoids occupied cells
    highscore.hpp / .cpp             JSON persistence via third_party/json.hpp
    replay.hpp / .cpp                 records/replays a round deterministically
    ai/
      pathfinding.hpp / .cpp         generic grid BFS / A* / flood fill
      hamiltonian.hpp / .cpp           builds a Hamiltonian cycle
      snake_ai.hpp / .cpp                the layered decision strategy
    window_context.hpp             RAII InitWindow/CloseWindow (see below)
    owned_texture.hpp               RAII LoadTexture/UnloadTexture
    sprite_sheet.hpp                 named sub-rects of one texture
    colors.hpp                        raylib Color constants
    renderer.hpp / .cpp                draws board/snake/food/HUD, no game logic
    audio_manager.hpp / .cpp             RAII sound loading, fails soft with no audio device
    input_handler.hpp / .cpp              raylib key polling -> game commands
    menu.hpp / .cpp                        hand-rolled start/game-over screens
    game.hpp / .cpp                          state machine + fixed-timestep loop
    main.cpp                                  parses --ai, builds and runs Game
  tests/                  one test_*.cpp per snake_core module, doctest
  assets/                 sprite sheet, sounds, icons (self-contained copy)
```

## Technical Decisions

### Why raylib instead of SDL2

raylib's `DrawTexturePro` takes a source rect and an arbitrarily-sized destination rect in one call, so scaling a 64x64 sprite-sheet cell to the game's 50px grid happens at draw time on the GPU. The Python/pygame version had to pre-crop *and* pre-scale every sprite into its own `Surface` up front, because `pygame.Surface.blit` doesn't scale. Same sprite-sheet-slicing idea as before, genuinely simpler in raylib.

### Construction order solves a real ownership problem

`SpriteSheet` and `OwnedTexture` call `LoadTexture`, which requires an OpenGL context to already exist -- which `InitWindow` creates. `Game` needs all of these as members, constructed together. C++ guarantees class members are constructed in **declaration order**, regardless of the order they're listed in a constructor's initializer list, so `game.hpp` declares a tiny `WindowContext` (RAII `InitWindow`/`CloseWindow`) as the *first* member of `Game` -- everything declared after it can safely assume a GL context exists. This isn't a workaround; it's the intended way to sequence RAII members with real dependencies between them.

### `snake_core` has no raylib include, on purpose

Every file under `ai/`, plus `snake.hpp`, `food.hpp`, and `highscore.hpp`, is a self-contained static library with zero rendering dependency. That's what makes `snake_tests` link and run in milliseconds without a display -- and it's not a testing-only convenience, it's the same boundary the Python version drew between pygame-free logic and pygame-dependent presentation, now enforced by the build graph instead of just by convention.

### The AI: why A* alone isn't good enough

The obvious approach is: every tick, run A* from the snake's head to the nearest apple, and follow it. This fails because A* only optimizes for *shortest path to food*, not *survives afterward* -- a snake can A\*-its-way into a pocket that's the shortest route to an apple but has no exit once its own body seals the entrance behind it.

`SnakeAI::Decide` (`src/ai/snake_ai.cpp`) layers four strategies, falling back in order:

1. **Safe A\*.** Find the A* path to the nearest food. Before committing, simulate walking the entire path -- including the length increase from eating -- and run a second BFS from the simulated head to the simulated tail. If the tail is still reachable, the rest of the board is provably still open, so the path is safe. If not, it's rejected even though it's the shortest one (`SafeAfter`, made public rather than private specifically so `tests/test_snake_ai.cpp` can exercise it directly against a hand-built trap grid).
2. **Tail chasing.** If no food path is safe, path toward the snake's own tail instead -- always reachable, since it's a cell about to be vacated.
3. **Max-space survival.** If even that fails, take whichever legal move leaves the largest reachable area via flood fill.
4. **Hamiltonian cycle (optional, `H` to toggle).** Follow a precomputed cycle that visits every cell exactly once and returns to the start -- can never trap itself, at the cost of ignoring food and filling the board slowly. Only exists for an even `GRID_COLS * GRID_ROWS` (a real bipartite-graph constraint, not an arbitrary limitation: see the comment in `hamiltonian.hpp`).

`tests/test_snake_ai.cpp` hand-builds a 3x2 grid where the direct A* path is a dead end, and asserts `SafeAfter` rejects it and `Decide` falls back to tail-chasing instead of taking the trap -- the regression test for the core design claim of the AI feature.

### Growth timing

Eating is resolved by checking whether the *next* head cell (via `Snake::PeekNextHead()`, before `Advance()`) is food, and calling `Grow()` first if so. `Advance()` only pops the tail when there's no pending growth, so calling `Grow()` first skips that pop on the very same tick as the eat, instead of the length increase lagging a tick behind.

### Dependency management

raylib is the only dependency that needs actual compilation, so it's the only one pulled in via CMake `FetchContent` (pinned to tag `5.5`). `doctest.h` and `nlohmann/json.hpp` are vendored as single headers in `third_party/` instead, which keeps configure time down and avoids depending on those projects' own CMake option surfaces for what's just a couple of headers.

### The AI debug overlay is a side channel, not a return value

`SnakeAI::Decide` still just returns a `Direction`. Introspection goes through a separate `AIDebugInfo` struct (`LastDebugInfo()`) that `Decide` populates as a side effect -- the considered A* path, whether it passed the safety check, and the flood-fill region behind a survival move. `Renderer::DrawAIDebug` draws it as translucent overlay cells (red for a rejected/unsafe path, green for a taken one, blue for the flood-fill region). Keeping this as an optional side channel instead of changing `Decide`'s signature means the AI's actual decision logic and its introspection don't leak into each other.

### Interpolation without touching the simulation

Rendering already runs at a different rate than the simulation ticks (`RenderFps` vs. `LogicTicksPerSecond`); interpolation is what makes that difference visible without lying to the simulation about it. `Game::PlayRound` snapshots the snake's body immediately before each tick's `Advance()`, and `Renderer::DrawSnake` linearly interpolates each segment's pixel position between that snapshot and the post-tick body, using `alpha = accumulator / stepSeconds` as the blend factor. `Snake` itself has no idea any of this is happening -- it still only ever occupies one discrete cell at a time; interpolation is purely a rendering-layer read of two consecutive ticks.

### Replay: record the decisions, not the frames

A round is fully determined by its start conditions (start cell/direction, the `FoodManager`'s RNG seed, apple count) plus the sequence of directions the snake actually committed to, one per tick -- so that's all `replay::Save` writes out, not raw input events or frame timing. `Game::PlayRound` always generates an explicit seed (`std::random_device`, no longer `FoodManager`'s internal default) specifically so it can be captured for the replay file if `--record` is set. Recording reads `Snake::CurrentDirection()` *after* `Advance()`, not the raw command that came in, so a replay reproduces what actually happened -- including a human/AI direction request that `Snake::SetDirection` silently rejected as an invalid reversal -- rather than what was merely requested.

## Future Work

- A hybrid AI mode that takes A* shortcuts across a Hamiltonian cycle when they're provably safe, instead of treating the two as separate, mutually exclusive modes.
- Mouse support in the menu (currently keyboard-only).
- Play back a replay at variable speed / frame-step through it, now that the render loop already separates "what tick are we on" from "what frame are we drawing."

## Credits

Snake spritesheet: https://opengameart.org/content/snake-sprite-sheet
