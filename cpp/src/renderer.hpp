// drawing only, no state mutation
#pragma once

#include <deque>
#include <vector>

#include "ai/snake_ai.hpp"
#include "cell.hpp"
#include "owned_texture.hpp"
#include "snake.hpp"
#include "sprite_sheet.hpp"

class Renderer {
public:
    Renderer(const SpriteSheet& sprites, const OwnedTexture& trophy);

    void DrawBoard() const;
    void DrawFood(const std::vector<Cell>& food) const;

    // interpolates segments between ticks for smooth motion; new tail
    // segment (no previous counterpart) draws at its exact position
    void DrawSnake(const std::deque<Cell>& previousBody, const Snake& snake, float alpha) const;

    void DrawHud(int score, int highScore, bool aiActive) const;
    void DrawAIDebug(const AIDebugInfo& debug) const;

private:
    const SpriteSheet& sprites_;
    const OwnedTexture& trophy_;
};
