#include "food.hpp"

#include <algorithm>

FoodManager::FoodManager(int cols, int rows, int count, unsigned int seed)
    : cols_(cols), rows_(rows), count_(count), rng_(seed) {}

void FoodManager::Reset(const std::unordered_set<Cell, CellHash>& occupied) {
    items_.clear();
    for (int i = 0; i < count_; ++i) {
        auto combined = occupied;
        for (const Cell& c : items_) combined.insert(c);
        SpawnOne(combined);
    }
}

bool FoodManager::SpawnOne(const std::unordered_set<Cell, CellHash>& occupied) {
    std::vector<Cell> free;
    free.reserve(static_cast<std::size_t>(cols_ * rows_));
    for (int x = 0; x < cols_; ++x) {
        for (int y = 0; y < rows_; ++y) {
            Cell c{x, y};
            if (occupied.find(c) == occupied.end()) free.push_back(c);
        }
    }
    if (free.empty()) return false;

    std::uniform_int_distribution<std::size_t> dist(0, free.size() - 1);
    items_.push_back(free[dist(rng_)]);
    return true;
}

bool FoodManager::Consume(const Cell& cell, const std::unordered_set<Cell, CellHash>& occupied) {
    auto it = std::find(items_.begin(), items_.end(), cell);
    if (it == items_.end()) return false;
    items_.erase(it);
    SpawnOne(occupied);
    return true;
}
