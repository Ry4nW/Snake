// one texture, named sub-rects, scaled on GPU at draw time
#pragma once

#include <raylib.h>

#include <stdexcept>
#include <string>
#include <unordered_map>

class SpriteSheet {
public:
    explicit SpriteSheet(const std::string& path) : texture_(LoadTexture(path.c_str())) {
        if (texture_.id == 0) {
            throw std::runtime_error("failed to load spritesheet: " + path);
        }
    }
    ~SpriteSheet() { UnloadTexture(texture_); }

    SpriteSheet(const SpriteSheet&) = delete;
    SpriteSheet& operator=(const SpriteSheet&) = delete;

    void Define(const std::string& name, Rectangle sourceRect) { rects_[name] = sourceRect; }

    // draws named sub-rect scaled to size at position
    void Draw(const std::string& name, Vector2 position, Vector2 size, Color tint = WHITE) const {
        Rectangle source = rects_.at(name);
        Rectangle dest{position.x, position.y, size.x, size.y};
        DrawTexturePro(texture_, source, dest, Vector2{0, 0}, 0.0f, tint);
    }

private:
    Texture2D texture_;
    std::unordered_map<std::string, Rectangle> rects_;
};
