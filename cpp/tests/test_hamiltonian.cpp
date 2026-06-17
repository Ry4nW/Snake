#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <unordered_set>

#include "ai/hamiltonian.hpp"
#include "doctest.h"

TEST_CASE("visits every cell exactly once") {
    int cols = 6, rows = 4;
    auto cycle = hamiltonian::BuildCycle(cols, rows);
    CHECK(cycle.size() == static_cast<std::size_t>(cols * rows));

    std::unordered_set<Cell, CellHash> unique(cycle.begin(), cycle.end());
    CHECK(unique.size() == static_cast<std::size_t>(cols * rows));
}

TEST_CASE("consecutive cells are grid-adjacent, including the wraparound") {
    auto cycle = hamiltonian::BuildCycle(6, 4);
    for (std::size_t i = 0; i < cycle.size(); ++i) {
        const Cell& a = cycle[i];
        const Cell& b = cycle[(i + 1) % cycle.size()];
        int manhattan = std::abs(a.col - b.col) + std::abs(a.row - b.row);
        CHECK(manhattan == 1);
    }
}

TEST_CASE("matches the actual game board dimensions") {
    auto cycle = hamiltonian::BuildCycle(18, 12);
    CHECK(cycle.size() == 18u * 12u);
}

TEST_CASE("rejects a grid with an odd number of cells") {
    CHECK_THROWS_AS(hamiltonian::BuildCycle(3, 3), std::invalid_argument);
}
