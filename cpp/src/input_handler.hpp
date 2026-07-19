// maps raylib keys to game commands
#pragma once

#include <vector>

#include "direction.hpp"

enum class CommandType { Direction, ToggleAi, ToggleAiMode, ToggleDebug, Quit };

struct Command {
    CommandType type;
    Direction direction = Direction::Right;  // only meaningful for type == Direction
};

class InputHandler {
public:
    std::vector<Command> Poll() const;
};
