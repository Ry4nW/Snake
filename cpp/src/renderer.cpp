#include "renderer.hpp"

#include "colors.hpp"
#include "config.hpp"

namespace {

const char* HeadSpriteFor(Direction d) {
    switch (d) {
        case Direction::Up: return "head_up";
        case Direction::Down: return "head_down";
        case Direction::Left: return "head_left";
        case Direction::Right: return "head_right";
    }
    return "head_right";
}

Vector2 ToVector2(config::PixelPos p) { return Vector2{static_cast<float>(p.x), static_cast<float>(p.y)}; }

}  // namespace

Renderer::Renderer(const SpriteSheet& sprites, const OwnedTexture& trophy)
    : sprites_(sprites), trophy_(trophy) {}

void Renderer::DrawBoard() const {
    ClearBackground(colors::Background);
    DrawRectangle(config::BoardMargin, config::BoardMargin, config::BoardWidth, config::BoardHeight,
                   colors::BoardLight);

    for (int row = 0; row < config::GridRows; ++row) {
        for (int col = 0; col < config::GridCols; ++col) {
            if ((row + col) % 2 == 0) continue;
            config::PixelPos pos = config::CellToPixel(Cell{col, row});
            DrawRectangle(pos.x, pos.y, config::CellSize, config::CellSize, colors::BoardDark);
        }
    }
}

void Renderer::DrawFood(const std::vector<Cell>& food) const {
    Vector2 size{static_cast<float>(config::CellSize), static_cast<float>(config::CellSize)};
    for (const Cell& cell : food) {
        sprites_.Draw("apple", ToVector2(config::CellToPixel(cell)), size);
    }
}

void Renderer::DrawSnake(const Snake& snake) const {
    Vector2 size{static_cast<float>(config::CellSize), static_cast<float>(config::CellSize)};
    const char* headSprite = HeadSpriteFor(snake.CurrentDirection());
    bool isHead = true;
    for (const Cell& cell : snake.Body()) {
        sprites_.Draw(isHead ? headSprite : "body", ToVector2(config::CellToPixel(cell)), size);
        isHead = false;
    }
}

void Renderer::DrawHud(int score, int highScore, bool aiActive) const {
    DrawText(TextFormat("%d", score), 45, 10, 30, WHITE);
    sprites_.Draw("apple", Vector2{75, 10}, Vector2{30, 30});

    const Texture2D& trophy = trophy_.Get();
    Rectangle trophySource{0, 0, static_cast<float>(trophy.width), static_cast<float>(trophy.height)};
    DrawTexturePro(trophy, trophySource, Rectangle{150, 10, 30, 30}, Vector2{0, 0}, 0.0f, WHITE);

    DrawText(TextFormat("%d", highScore), 185, 10, 30, WHITE);

    if (aiActive) {
        DrawText("AI", 900, 10, 30, colors::AiTag);
    }
}
