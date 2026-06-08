// builds a cycle visiting every cell once, back to start
// snake can never trap itself following it, but ignores food
//
// needs rows * cols even (bipartite parity constraint)
#pragma once

#include <vector>

#include "../cell.hpp"

namespace hamiltonian {

// throws std::invalid_argument if cols * rows is odd
std::vector<Cell> BuildCycle(int cols, int rows);

}  // namespace hamiltonian
