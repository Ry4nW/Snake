#include "hamiltonian.hpp"

#include <stdexcept>

namespace hamiltonian {

std::vector<Cell> BuildCycle(int cols, int rows) {
    if ((cols * rows) % 2 != 0) {
        throw std::invalid_argument("a Hamiltonian cycle needs an even number of cells");
    }

    std::vector<Cell> cycle;
    cycle.reserve(static_cast<std::size_t>(cols * rows));

    // column 0 is the return lane, top to bottom
    for (int row = 0; row < rows; ++row) cycle.push_back(Cell{0, row});

    // other columns snake up/down, skip row 0 for the return lane
    for (int col = 1; col < cols; ++col) {
        if (col % 2 == 1) {
            for (int row = rows - 1; row > 0; --row) cycle.push_back(Cell{col, row});
        } else {
            for (int row = 1; row < rows; ++row) cycle.push_back(Cell{col, row});
        }
    }

    // close the loop along row 0
    for (int col = cols - 1; col > 0; --col) cycle.push_back(Cell{col, 0});

    return cycle;
}

}  // namespace hamiltonian
