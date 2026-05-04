// grid size, timing, colors, all in one place
#pragma once

#include "cell.hpp"

namespace config {

constexpr int CellSize = 50;
constexpr int GridCols = 18;
constexpr int GridRows = 12;
constexpr int BoardMargin = 50;

constexpr int BoardWidth = GridCols * CellSize;
constexpr int BoardHeight = GridRows * CellSize;
constexpr int WindowWidth = BoardWidth + 2 * BoardMargin;
constexpr int WindowHeight = BoardHeight + 2 * BoardMargin;

// render at RenderFps, logic steps via accumulator, rates stay independent
constexpr int RenderFps = 60;
constexpr double LogicTicksPerSecond = 7.0;

constexpr int FewApples = 1;
constexpr int ManyApples = 3;

constexpr const char* HighScoreFile = "highscore.json";

struct PixelPos {
    int x;
    int y;
};

// only place a cell becomes a screen coord, no raylib dep
inline PixelPos CellToPixel(const Cell& cell) {
    return PixelPos{BoardMargin + cell.col * CellSize, BoardMargin + cell.row * CellSize};
}

}  // namespace config
