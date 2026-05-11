// owns food placement, keeps spawn logic out of the game loop
#pragma once

#include <random>
#include <unordered_set>
#include <vector>

#include "cell.hpp"

class FoodManager {
public:
    FoodManager(int cols, int rows, int count, unsigned int seed = std::random_device{}());

    const std::vector<Cell>& Items() const { return items_; }

    void Reset(const std::unordered_set<Cell, CellHash>& occupied);
    bool SpawnOne(const std::unordered_set<Cell, CellHash>& occupied);
    bool Consume(const Cell& cell, const std::unordered_set<Cell, CellHash>& occupied);

private:
    int cols_;
    int rows_;
    int count_;
    std::vector<Cell> items_;
    std::mt19937 rng_;
};
