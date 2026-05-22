#include "input_handler.hpp"

#include <raylib.h>

std::vector<Command> InputHandler::Poll() const {
    std::vector<Command> commands;

    if (WindowShouldClose()) commands.push_back(Command{CommandType::Quit});

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        commands.push_back(Command{CommandType::Direction, Direction::Left});
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        commands.push_back(Command{CommandType::Direction, Direction::Right});
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        commands.push_back(Command{CommandType::Direction, Direction::Up});
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        commands.push_back(Command{CommandType::Direction, Direction::Down});
    }
    if (IsKeyPressed(KEY_TAB)) commands.push_back(Command{CommandType::ToggleAi});
    if (IsKeyPressed(KEY_H)) commands.push_back(Command{CommandType::ToggleAiMode});

    return commands;
}
