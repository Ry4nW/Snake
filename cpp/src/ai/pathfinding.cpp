#include "pathfinding.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <queue>
#include <unordered_map>

namespace pathfinding {

bool InBounds(const Cell& c, int cols, int rows) {
    return c.col >= 0 && c.col < cols && c.row >= 0 && c.row < rows;
}

std::vector<Cell> Neighbors(const Cell& c, int cols, int rows) {
    static constexpr std::array<Cell, 4> kDeltas{Cell{1, 0}, Cell{-1, 0}, Cell{0, 1}, Cell{0, -1}};
    std::vector<Cell> result;
    result.reserve(4);
    for (const Cell& d : kDeltas) {
        Cell next{c.col + d.col, c.row + d.row};
        if (InBounds(next, cols, rows)) result.push_back(next);
    }
    return result;
}

namespace {

std::vector<Cell> Reconstruct(const std::unordered_map<Cell, Cell, CellHash>& cameFrom, const Cell& start,
                               const Cell& goal) {
    std::vector<Cell> path{goal};
    while (path.back() != start) {
        path.push_back(cameFrom.at(path.back()));
    }
    std::reverse(path.begin(), path.end());
    return path;
}

}  // namespace

std::optional<std::vector<Cell>> BfsPath(const Cell& start, const Cell& goal,
                                          const std::unordered_set<Cell, CellHash>& blocked, int cols,
                                          int rows) {
    if (start == goal) return std::vector<Cell>{start};

    std::unordered_set<Cell, CellHash> visited{start};
    std::queue<Cell> frontier;
    frontier.push(start);
    std::unordered_map<Cell, Cell, CellHash> cameFrom;

    while (!frontier.empty()) {
        Cell current = frontier.front();
        frontier.pop();
        for (const Cell& next : Neighbors(current, cols, rows)) {
            if (visited.count(next) || blocked.count(next)) continue;
            visited.insert(next);
            cameFrom[next] = current;
            if (next == goal) return Reconstruct(cameFrom, start, goal);
            frontier.push(next);
        }
    }
    return std::nullopt;
}

std::optional<std::vector<Cell>> AStarPath(const Cell& start, const Cell& goal,
                                            const std::unordered_set<Cell, CellHash>& blocked, int cols,
                                            int rows) {
    auto heuristic = [](const Cell& a, const Cell& b) {
        return std::abs(a.col - b.col) + std::abs(a.row - b.row);
    };

    struct Node {
        int f;
        int g;
        long counter;  // tie-breaker, avoids comparing Cells
        Cell cell;
    };
    struct Compare {
        bool operator()(const Node& a, const Node& b) const {
            if (a.f != b.f) return a.f > b.f;
            return a.counter > b.counter;
        }
    };

    std::priority_queue<Node, std::vector<Node>, Compare> open;
    std::unordered_map<Cell, Cell, CellHash> cameFrom;
    std::unordered_map<Cell, int, CellHash> bestG{{start, 0}};
    std::unordered_set<Cell, CellHash> closed;

    long counter = 0;
    open.push(Node{heuristic(start, goal), 0, counter++, start});

    while (!open.empty()) {
        Node current = open.top();
        open.pop();
        if (closed.count(current.cell)) continue;
        if (current.cell == goal) return Reconstruct(cameFrom, start, goal);
        closed.insert(current.cell);

        for (const Cell& next : Neighbors(current.cell, cols, rows)) {
            if (blocked.count(next) || closed.count(next)) continue;
            int tentativeG = current.g + 1;
            auto it = bestG.find(next);
            if (it == bestG.end() || tentativeG < it->second) {
                bestG[next] = tentativeG;
                cameFrom[next] = current.cell;
                open.push(Node{tentativeG + heuristic(next, goal), tentativeG, counter++, next});
            }
        }
    }
    return std::nullopt;
}

std::vector<Cell> FloodFillRegion(const Cell& start, const std::unordered_set<Cell, CellHash>& blocked,
                                   int cols, int rows) {
    if (blocked.count(start)) return {};

    std::unordered_set<Cell, CellHash> visited{start};
    std::queue<Cell> frontier;
    frontier.push(start);
    std::vector<Cell> region;

    while (!frontier.empty()) {
        Cell current = frontier.front();
        frontier.pop();
        region.push_back(current);
        for (const Cell& next : Neighbors(current, cols, rows)) {
            if (visited.count(next) || blocked.count(next)) continue;
            visited.insert(next);
            frontier.push(next);
        }
    }
    return region;
}

int FloodFillCount(const Cell& start, const std::unordered_set<Cell, CellHash>& blocked, int cols,
                    int rows) {
    return static_cast<int>(FloodFillRegion(start, blocked, cols, rows).size());
}

Direction DirectionBetween(const Cell& a, const Cell& b) {
    Cell delta{b.col - a.col, b.row - a.row};
    if (delta.col == 1) return Direction::Right;
    if (delta.col == -1) return Direction::Left;
    if (delta.row == 1) return Direction::Down;
    return Direction::Up;
}

}  // namespace pathfinding
