// game state machine, fixed-timestep loop
#pragma once

#include "ai/snake_ai.hpp"
#include "audio_manager.hpp"
#include "food.hpp"
#include "highscore.hpp"
#include "input_handler.hpp"
#include "menu.hpp"
#include "owned_texture.hpp"
#include "renderer.hpp"
#include "snake.hpp"
#include "sprite_sheet.hpp"
#include "window_context.hpp"

class Game {
public:
    explicit Game(bool startWithAi);

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

    int lastScore_ = 0;

    void PlayRound();
    void Render(const Snake& snake, const FoodManager& food, bool aiActive);
};
