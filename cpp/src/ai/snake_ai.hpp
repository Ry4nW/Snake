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

class SnakeAI {
public:
    SnakeAI(int cols, int rows);

    std::optional<Direction> Decide(const Snake& snake, const std::vector<Cell>& food,
                                     bool useHamiltonian = false);

    // public so tests can hit it directly with a hand-built trap grid
    bool SafeAfter(const std::deque<Cell>& body, const std::vector<Cell>& path) const;

private:
    int cols_;
    int rows_;
    std::vector<Cell> cycle_;
    std::unordered_map<Cell, std::size_t, CellHash> cycleIndex_;

    std::optional<Cell> Nearest(const Cell& head, const std::vector<Cell>& food) const;
    std::deque<Cell> SimulatePath(const std::deque<Cell>& body, const std::vector<Cell>& path) const;
    std::optional<Direction> ChaseTail(const std::deque<Cell>& body) const;
    std::optional<Direction> SafestMove(const std::deque<Cell>& body) const;
    std::optional<Direction> DecideHamiltonian(const Snake& snake);
};
