// game state machine, fixed-timestep loop
#pragma once

#include <deque>
#include <optional>
#include <string>

#include "ai/snake_ai.hpp"
#include "audio_manager.hpp"
#include "food.hpp"
#include "highscore.hpp"
#include "input_handler.hpp"
#include "menu.hpp"
#include "owned_texture.hpp"
#include "renderer.hpp"
#include "replay.hpp"
#include "snake.hpp"
#include "sprite_sheet.hpp"
#include "window_context.hpp"

class Game {
public:
    // recordPath: save first round as replay. replayPath: play back once, then exit
    Game(bool startWithAi, std::optional<std::string> recordPath = std::nullopt,
         std::optional<std::string> replayPath = std::nullopt);

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void Run();

private:
    // declaration order matters: window must exist before textures load
    WindowContext window_;
    GameSettings settings_;
    SpriteSheet sprites_;
    OwnedTexture trophy_;
    AudioManager audio_;
    HighScoreStore highScore_;
    InputHandler input_;
    Renderer renderer_;
    MenuController menu_;
    SnakeAI ai_;

    std::optional<std::string> recordPath_;
    std::optional<std::string> replayPath_;
    bool recorded_ = false;
    int lastScore_ = 0;

    void PlayRound();
    void PlayReplay(const std::string& path);
    void Render(const std::deque<Cell>& previousBody, const Snake& snake, const FoodManager& food,
                bool aiActive, bool showDebug, float alpha);
};
