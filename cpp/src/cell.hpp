// grid cell, only Renderer converts to pixels
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

struct Cell {
    int col = 0;
    int row = 0;

    friend bool operator==(const Cell& a, const Cell& b) {
        return a.col == b.col && a.row == b.row;
    }
    friend bool operator!=(const Cell& a, const Cell& b) { return !(a == b); }
};

struct CellHash {
    std::size_t operator()(const Cell& c) const noexcept {
        // packs col/row into a 64-bit key
        std::uint64_t packed = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(c.col)) << 32) |
                                static_cast<std::uint32_t>(c.row);
        return std::hash<std::uint64_t>{}(packed);
    }
};
