#include "game.hpp"

#include <raylib.h>

#include <algorithm>
#include <unordered_set>

#include "config.hpp"

namespace {

std::unordered_set<Cell, CellHash> ToSet(const std::deque<Cell>& body) {
    return std::unordered_set<Cell, CellHash>(body.begin(), body.end());
}

}  // namespace

Game::Game(bool startWithAi)
    : window_(config::WindowWidth, config::WindowHeight, "Snake"),
      sprites_("assets/snake_spritesheet.png"),
      trophy_("assets/trophy.png"),
      highScore_(config::HighScoreFile),
      renderer_(sprites_, trophy_),
      ai_(config::GridCols, config::GridRows) {
    SetTargetFPS(config::RenderFps);

    Image icon = LoadImage("assets/snake_game_icon.jpg");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

    sprites_.Define("apple", Rectangle{0, 192, 64, 64});
    sprites_.Define("head_left", Rectangle{192, 64, 64, 64});
    sprites_.Define("head_right", Rectangle{256, 0, 64, 64});
    sprites_.Define("head_up", Rectangle{192, 0, 64, 64});
    sprites_.Define("head_down", Rectangle{256, 64, 64, 64});
    sprites_.Define("body", Rectangle{64, 0, 64, 64});

    settings_.aiEnabled = startWithAi;
}

void Game::Run() {
    if (!menu_.ShowStart(settings_)) return;

    while (true) {
        PlayRound();
        if (!menu_.ShowGameOver(lastScore_, highScore_.Value())) return;
    }
}

void Game::PlayRound() {
    Snake snake(Cell{3, 5}, Direction::Right);
    FoodManager food(config::GridCols, config::GridRows, settings_.appleCount);
    food.Reset(ToSet(snake.Body()));

    bool aiActive = settings_.aiEnabled;
    bool useHamiltonian = false;
    bool started = aiActive;  // human needs a keypress first, AI doesn't wait

    double accumulator = 0.0;
    const double stepSeconds = 1.0 / config::LogicTicksPerSecond;

    while (!WindowShouldClose()) {
        accumulator += GetFrameTime();

        for (const Command& command : input_.Poll()) {
            switch (command.type) {
                case CommandType::Quit:
                    return;
                case CommandType::ToggleAi:
                    aiActive = !aiActive;
                    started = true;
                    break;
                case CommandType::ToggleAiMode:
                    useHamiltonian = !useHamiltonian;
                    break;
                case CommandType::Direction:
                    if (!aiActive) {
                        snake.SetDirection(command.direction);
                        started = true;
                    }
                    break;
            }
        }

        while (accumulator >= stepSeconds) {
            accumulator -= stepSeconds;
            if (!started) continue;

            if (aiActive) {
                auto direction = ai_.Decide(snake, food.Items(), useHamiltonian);
                if (direction) snake.SetDirection(*direction);
            }

            Cell nextHead = snake.PeekNextHead();
            const auto& items = food.Items();
            bool eating = std::find(items.begin(), items.end(), nextHead) != items.end();
            if (eating) snake.Grow();
            snake.Advance();

            if (snake.HitsWall(config::GridCols, config::GridRows) || snake.HitsSelf()) {
                audio_.PlayDeath();
                lastScore_ = snake.Length() - 1;
                highScore_.Update(lastScore_);
                return;
            }

            if (eating) {
                audio_.PlayEat();
                food.Consume(nextHead, ToSet(snake.Body()));
            }
        }

        Render(snake, food, aiActive);
    }
}

void Game::Render(const Snake& snake, const FoodManager& food, bool aiActive) {
    BeginDrawing();
    renderer_.DrawBoard();
    renderer_.DrawFood(food.Items());
    renderer_.DrawSnake(snake);
    renderer_.DrawHud(snake.Length() - 1, highScore_.Value(), aiActive);
    EndDrawing();
}
