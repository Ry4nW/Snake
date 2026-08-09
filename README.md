# Snake

Grid based snake game with an AI that can play itself. It uses A* to find the food, but checks first that taking that path won't trap the snake before committing to it. If nothing's safe it falls back to chasing its own tail, then to just picking whatever move leaves the most open space. There's also a Hamiltonian cycle mode that never loses, if you want a slower guaranteed win instead.

Main version is in `cpp/`, built with C++20 and raylib. There's an older python/pygame version in `python/` that I don't really touch anymore, kept around for reference.

## Quick start (C++)

```
cmake -B cpp/build -S cpp
cmake --build cpp/build -j
./cpp/build/snake_cpp --ai
```

More detail on controls and how the AI works is in [cpp/README.md](cpp/README.md).

## Layout

```
cpp/        main version - C++20, raylib, CMake, doctest
python/     old prototype - python, pygame
```

Both have their own README with more detail on that version specifically.
