#include "game.hpp"

#include <raylib.h>

#include <algorithm>
#include <random>
#include <unordered_set>

#include "config.hpp"

namespace {

std::unordered_set<Cell, CellHash> ToSet(const std::deque<Cell>& body) {
    return std::unordered_set<Cell, CellHash>(body.begin(), body.end());
}

}  // namespace

Game::Game(bool startWithAi, std::optional<std::string> recordPath, std::optional<std::string> replayPath)
    : window_(config::WindowWidth, config::WindowHeight, "Snake"),
      sprites_("assets/snake_spritesheet.png"),
      trophy_("assets/trophy.png"),
      highScore_(config::HighScoreFile),
      renderer_(sprites_, trophy_),
      ai_(config::GridCols, config::GridRows),
      recordPath_(std::move(recordPath)),
      replayPath_(std::move(replayPath)) {
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
    if (replayPath_) {
        PlayReplay(*replayPath_);
        return;
    }

    if (!menu_.ShowStart(settings_)) return;

    while (true) {
        PlayRound();
        if (!menu_.ShowGameOver(lastScore_, highScore_.Value())) return;
    }
}

void Game::PlayRound() {
    const Cell startCell{3, 5};
    const Direction startDirection = Direction::Right;
    const unsigned int foodSeed = std::random_device{}();

    Snake snake(startCell, startDirection);
    FoodManager food(config::GridCols, config::GridRows, settings_.appleCount, foodSeed);
    food.Reset(ToSet(snake.Body()));
    std::vector<Direction> recordedDirections;

    bool aiActive = settings_.aiEnabled;
    bool useHamiltonian = false;
    bool showDebug = false;
    bool started = aiActive;  // human needs a keypress first, AI doesn't wait

    double accumulator = 0.0;
    const double stepSeconds = 1.0 / config::LogicTicksPerSecond;
    std::deque<Cell> previousBody = snake.Body();

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
                case CommandType::ToggleDebug:
                    showDebug = !showDebug;
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

            // snapshot pre-tick body so Render() can interpolate to it
            previousBody = snake.Body();

            if (aiActive) {
                auto direction = ai_.Decide(snake, food.Items(), useHamiltonian);
                if (direction) snake.SetDirection(*direction);
            }

            Cell nextHead = snake.PeekNextHead();
            const auto& items = food.Items();
            bool eating = std::find(items.begin(), items.end(), nextHead) != items.end();
            if (eating) snake.Grow();
            snake.Advance();

            // record after Advance() so it's what actually happened, not what was requested
            if (recordPath_) recordedDirections.push_back(snake.CurrentDirection());

            if (snake.HitsWall(config::GridCols, config::GridRows) || snake.HitsSelf()) {
                audio_.PlayDeath();
                lastScore_ = snake.Length() - 1;
                highScore_.Update(lastScore_);
                if (recordPath_ && !recorded_) {
                    replay::Save(*recordPath_,
                                 ReplayData{startCell, startDirection, foodSeed, settings_.appleCount,
                                            recordedDirections});
                    recorded_ = true;
                }
                return;
            }

            if (eating) {
                audio_.PlayEat();
                food.Consume(nextHead, ToSet(snake.Body()));
            }
        }

        float alpha = static_cast<float>(accumulator / stepSeconds);
        Render(previousBody, snake, food, aiActive, showDebug, alpha);
    }
}

void Game::PlayReplay(const std::string& path) {
    ReplayData data = replay::Load(path);

    Snake snake(data.startCell, data.startDirection);
    FoodManager food(config::GridCols, config::GridRows, data.appleCount, data.foodSeed);
    food.Reset(ToSet(snake.Body()));

    std::size_t cursor = 0;
    double accumulator = 0.0;
    const double stepSeconds = 1.0 / config::LogicTicksPerSecond;
    std::deque<Cell> previousBody = snake.Body();

    while (!WindowShouldClose()) {
        accumulator += GetFrameTime();

        for (const Command& command : input_.Poll()) {
            if (command.type == CommandType::Quit) return;
        }

        while (accumulator >= stepSeconds) {
            accumulator -= stepSeconds;

            if (cursor >= data.directions.size()) return;  // replay finished

            previousBody = snake.Body();
            snake.SetDirection(data.directions[cursor++]);

            Cell nextHead = snake.PeekNextHead();
            const auto& items = food.Items();
            bool eating = std::find(items.begin(), items.end(), nextHead) != items.end();
            if (eating) snake.Grow();
            snake.Advance();

            if (snake.HitsWall(config::GridCols, config::GridRows) || snake.HitsSelf()) {
                audio_.PlayDeath();
                return;
            }

            if (eating) {
                audio_.PlayEat();
                food.Consume(nextHead, ToSet(snake.Body()));
            }
        }

        float alpha = static_cast<float>(accumulator / stepSeconds);
        Render(previousBody, snake, food, /*aiActive=*/false, /*showDebug=*/false, alpha);
    }
}

void Game::Render(const std::deque<Cell>& previousBody, const Snake& snake, const FoodManager& food,
                   bool aiActive, bool showDebug, float alpha) {
    BeginDrawing();
    renderer_.DrawBoard();
    if (showDebug && aiActive) {
        renderer_.DrawAIDebug(ai_.LastDebugInfo());
    }
    renderer_.DrawFood(food.Items());
    renderer_.DrawSnake(previousBody, snake, alpha);
    renderer_.DrawHud(snake.Length() - 1, highScore_.Value(), aiActive);
    EndDrawing();
}
