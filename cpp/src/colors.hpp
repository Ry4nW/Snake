#pragma once

#include <raylib.h>

namespace colors {

constexpr Color Background{95, 161, 85, 255};
constexpr Color BoardLight{170, 227, 104, 255};
constexpr Color BoardDark{103, 145, 54, 255};
constexpr Color AiTag{255, 215, 0, 255};
constexpr Color Highlight{255, 215, 0, 255};

// AI debug overlay
constexpr Color DebugFloodFill{80, 160, 255, 90};
constexpr Color DebugPathSafe{60, 220, 100, 140};
constexpr Color DebugPathUnsafe{230, 60, 60, 140};
constexpr Color DebugTailChase{230, 180, 60, 140};

}  // namespace colors
