// RAII InitWindow/CloseWindow, declared first in Game so it's built before
// anything needing a GL context (construction follows declaration order)
#pragma once

#include <raylib.h>

class WindowContext {
public:
    WindowContext(int width, int height, const char* title) { InitWindow(width, height, title); }
    ~WindowContext() { CloseWindow(); }

    WindowContext(const WindowContext&) = delete;
    WindowContext& operator=(const WindowContext&) = delete;
};
