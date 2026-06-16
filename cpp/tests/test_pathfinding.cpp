#include "ai/pathfinding.hpp"
#include "doctest.h"

using pathfinding::AStarPath;
using pathfinding::BfsPath;
using pathfinding::DirectionBetween;
using pathfinding::FloodFillCount;

TEST_CASE("bfs finds the shortest path on an open grid") {
    auto path = BfsPath(Cell{0, 0}, Cell{2, 0}, {}, 5, 5);
    REQUIRE(path.has_value());
    std::vector<Cell> expected{Cell{0, 0}, Cell{1, 0}, Cell{2, 0}};
    CHECK(*path == expected);
}

TEST_CASE("bfs routes around obstacles") {
    std::unordered_set<Cell, CellHash> blocked{Cell{1, 0}, Cell{1, 1}};
    auto path = BfsPath(Cell{0, 0}, Cell{2, 0}, blocked, 3, 3);
    REQUIRE(path.has_value());
    for (const Cell& c : *path) CHECK(blocked.count(c) == 0);
}

TEST_CASE("bfs returns nothing when the goal is walled off") {
    std::unordered_set<Cell, CellHash> blocked;
    for (int y = 0; y < 5; ++y) blocked.insert(Cell{1, y});
    CHECK_FALSE(BfsPath(Cell{0, 0}, Cell{4, 0}, blocked, 5, 5).has_value());
}

TEST_CASE("astar matches bfs path length on an open grid") {
    auto bfs = BfsPath(Cell{0, 0}, Cell{4, 4}, {}, 5, 5);
    auto astar = AStarPath(Cell{0, 0}, Cell{4, 4}, {}, 5, 5);
    REQUIRE(bfs.has_value());
    REQUIRE(astar.has_value());
    CHECK(bfs->size() == astar->size());
}

TEST_CASE("astar returns nothing when unreachable") {
    std::unordered_set<Cell, CellHash> blocked;
    for (int y = 0; y < 5; ++y) blocked.insert(Cell{1, y});
    CHECK_FALSE(AStarPath(Cell{0, 0}, Cell{4, 0}, blocked, 5, 5).has_value());
}

TEST_CASE("flood fill counts an enclosed region correctly") {
    // A wall at col 2 splits a 5x5 grid into two 2-wide pockets.
    std::unordered_set<Cell, CellHash> blocked;
    for (int y = 0; y < 5; ++y) blocked.insert(Cell{2, y});
    CHECK(FloodFillCount(Cell{0, 0}, blocked, 5, 5) == 10);
}

TEST_CASE("flood fill returns zero when start is blocked") {
    CHECK(FloodFillCount(Cell{0, 0}, {Cell{0, 0}}, 5, 5) == 0);
}

TEST_CASE("direction between adjacent cells") {
    CHECK(DirectionBetween(Cell{3, 3}, Cell{4, 3}) == Direction::Right);
    CHECK(DirectionBetween(Cell{3, 3}, Cell{3, 2}) == Direction::Up);
}
