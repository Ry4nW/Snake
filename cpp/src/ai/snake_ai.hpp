// decision logic for the AI snake
//
// naive A* to nearest food can trap the snake, seals off its own escape
//
// fallback order:
//   1. A* to food, simulate path + growth, verify tail still reachable
//   2. chase own tail, always reachable, stalls safely
//   3. legal move with most reachable space (flood fill)
//   4. optional: hamiltonian cycle, never traps but ignores food
#pragma once

#include <deque>
#include <optional>
#include <unordered_map>
#include <vector>

#include "../cell.hpp"
#include "../direction.hpp"
#include "../snake.hpp"

// snapshot of what the AI considered last decision, for debug overlay/tests
struct AIDebugInfo {
    enum class Strategy { None, SafeAStar, TailChase, FloodFillSurvival, Hamiltonian };

    Strategy strategy = Strategy::None;

    // A* path to food evaluated this tick, taken or not
    std::vector<Cell> consideredPath;
    bool consideredPathSafe = false;

    // path actually walked, for SafeAStar/TailChase
    std::vector<Cell> chosenPath;

    // reachable region behind a FloodFillSurvival pick
    std::vector<Cell> floodFillRegion;
};

class SnakeAI {
public:
    SnakeAI(int cols, int rows);

    std::optional<Direction> Decide(const Snake& snake, const std::vector<Cell>& food,
                                     bool useHamiltonian = false);

    // public so tests can hit it directly with a hand-built trap grid
    bool SafeAfter(const std::deque<Cell>& body, const std::vector<Cell>& path) const;

    const AIDebugInfo& LastDebugInfo() const { return debug_; }

private:
    int cols_;
    int rows_;
    std::vector<Cell> cycle_;
    std::unordered_map<Cell, std::size_t, CellHash> cycleIndex_;
    AIDebugInfo debug_;

    std::optional<Cell> Nearest(const Cell& head, const std::vector<Cell>& food) const;
    std::deque<Cell> SimulatePath(const std::deque<Cell>& body, const std::vector<Cell>& path) const;
    std::optional<Direction> ChaseTail(const std::deque<Cell>& body);
    std::optional<Direction> SafestMove(const std::deque<Cell>& body);
    std::optional<Direction> DecideHamiltonian(const Snake& snake);
};
