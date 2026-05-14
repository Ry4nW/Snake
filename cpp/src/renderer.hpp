// drawing only, no state mutation
#pragma once

#include <vector>

#include "cell.hpp"
#include "owned_texture.hpp"
#include "snake.hpp"
#include "sprite_sheet.hpp"

class Renderer {
public:
    Renderer(const SpriteSheet& sprites, const OwnedTexture& trophy);

    void DrawBoard() const;
    void DrawFood(const std::vector<Cell>& food) const;
    void DrawSnake(const Snake& snake) const;
    void DrawHud(int score, int highScore, bool aiActive) const;

private:
    const SpriteSheet& sprites_;
    const OwnedTexture& trophy_;
};
