// generic grid pathfinding, no knowledge of snakes or food
#pragma once

#include <optional>
#include <unordered_set>
#include <vector>

#include "../cell.hpp"
#include "../direction.hpp"

namespace pathfinding {

bool InBounds(const Cell& c, int cols, int rows);
std::vector<Cell> Neighbors(const Cell& c, int cols, int rows);

// shortest path start to goal, nullopt if unreachable, includes both ends
std::optional<std::vector<Cell>> BfsPath(const Cell& start, const Cell& goal,
                                          const std::unordered_set<Cell, CellHash>& blocked, int cols,
                                          int rows);

// same result as bfs, expands fewer nodes
std::optional<std::vector<Cell>> AStarPath(const Cell& start, const Cell& goal,
                                            const std::unordered_set<Cell, CellHash>& blocked, int cols,
                                            int rows);

// all cells reachable from start, start included
std::vector<Cell> FloodFillRegion(const Cell& start, const std::unordered_set<Cell, CellHash>& blocked,
                                   int cols, int rows);

// reachable cell count, cheap proxy for available space
int FloodFillCount(const Cell& start, const std::unordered_set<Cell, CellHash>& blocked, int cols,
                    int rows);

Direction DirectionBetween(const Cell& a, const Cell& b);

}  // namespace pathfinding
