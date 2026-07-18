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

Vector2 LerpPixel(const Cell& from, const Cell& to, float alpha) {
    config::PixelPos p0 = config::CellToPixel(from);
    config::PixelPos p1 = config::CellToPixel(to);
    return Vector2{
        static_cast<float>(p0.x) + (static_cast<float>(p1.x) - static_cast<float>(p0.x)) * alpha,
        static_cast<float>(p0.y) + (static_cast<float>(p1.y) - static_cast<float>(p0.y)) * alpha,
    };
}

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

void Renderer::DrawSnake(const std::deque<Cell>& previousBody, const Snake& snake, float alpha) const {
    const std::deque<Cell>& currentBody = snake.Body();
    Vector2 size{static_cast<float>(config::CellSize), static_cast<float>(config::CellSize)};
    const char* headSprite = HeadSpriteFor(snake.CurrentDirection());

    for (std::size_t i = 0; i < currentBody.size(); ++i) {
        Vector2 position = (i < previousBody.size()) ? LerpPixel(previousBody[i], currentBody[i], alpha)
                                                       : ToVector2(config::CellToPixel(currentBody[i]));
        sprites_.Draw(i == 0 ? headSprite : "body", position, size);
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

void Renderer::DrawAIDebug(const AIDebugInfo& debug) const {
    Vector2 size{static_cast<float>(config::CellSize), static_cast<float>(config::CellSize)};

    for (const Cell& cell : debug.floodFillRegion) {
        DrawRectangleV(ToVector2(config::CellToPixel(cell)), size, colors::DebugFloodFill);
    }

    if (!debug.consideredPathSafe) {
        for (const Cell& cell : debug.consideredPath) {
            DrawRectangleV(ToVector2(config::CellToPixel(cell)), size, colors::DebugPathUnsafe);
        }
    }

    Color chosenColor =
        debug.strategy == AIDebugInfo::Strategy::TailChase ? colors::DebugTailChase : colors::DebugPathSafe;
    for (const Cell& cell : debug.chosenPath) {
        DrawRectangleV(ToVector2(config::CellToPixel(cell)), size, chosenColor);
    }
}
