// hand-rolled menu, raylib has no widget lib, just two screens
#pragma once

#include "config.hpp"

struct GameSettings {
    int appleCount = config::FewApples;
    bool aiEnabled = false;
};

class MenuController {
public:
    // false if window closed instead of Play chosen
    bool ShowStart(GameSettings& settings) const;

    // true to play again, false to quit
    bool ShowGameOver(int score, int highScore) const;
};
