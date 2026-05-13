// RAII wrapper for a standalone texture (trophy icon), needs GL context first
#pragma once

#include <raylib.h>

#include <stdexcept>
#include <string>

class OwnedTexture {
public:
    explicit OwnedTexture(const std::string& path) : texture_(LoadTexture(path.c_str())) {
        if (texture_.id == 0) {
            throw std::runtime_error("failed to load texture: " + path);
        }
    }
    ~OwnedTexture() { UnloadTexture(texture_); }

    OwnedTexture(const OwnedTexture&) = delete;
    OwnedTexture& operator=(const OwnedTexture&) = delete;

    const Texture2D& Get() const { return texture_; }

private:
    Texture2D texture_;
};
