#pragma once

#include "cell.hpp"

enum class Direction { Up, Down, Left, Right };

inline Cell DirectionDelta(Direction d) {
    switch (d) {
        case Direction::Up: return Cell{0, -1};
        case Direction::Down: return Cell{0, 1};
        case Direction::Left: return Cell{-1, 0};
        case Direction::Right: return Cell{1, 0};
    }
    return Cell{0, 0};
}

inline Direction Opposite(Direction d) {
    switch (d) {
        case Direction::Up: return Direction::Down;
        case Direction::Down: return Direction::Up;
        case Direction::Left: return Direction::Right;
        case Direction::Right: return Direction::Left;
    }
    return d;
}
