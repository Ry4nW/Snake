// deterministic recording/playback of a round
//
// start conditions + per-tick direction sequence fully determine a run,
// so that's all we record, no timing or raw input events needed
#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "cell.hpp"
#include "direction.hpp"

struct ReplayData {
    Cell startCell{};
    Direction startDirection = Direction::Right;
    unsigned int foodSeed = 0;
    int appleCount = 1;
    std::vector<Direction> directions;
};

namespace replay {

void Save(const std::string& path, const ReplayData& data);

// throws std::runtime_error if file can't be read or parsed
ReplayData Load(const std::string& path);

}  // namespace replay
