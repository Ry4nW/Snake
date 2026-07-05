#include "replay.hpp"

#include <fstream>

#include "json.hpp"

namespace {

const char* DirectionName(Direction d) {
    switch (d) {
        case Direction::Up: return "Up";
        case Direction::Down: return "Down";
        case Direction::Left: return "Left";
        case Direction::Right: return "Right";
    }
    return "Right";
}

Direction DirectionFromName(const std::string& name) {
    if (name == "Up") return Direction::Up;
    if (name == "Down") return Direction::Down;
    if (name == "Left") return Direction::Left;
    return Direction::Right;
}

}  // namespace

namespace replay {

void Save(const std::string& path, const ReplayData& data) {
    nlohmann::json j;
    j["start_cell"] = {{"col", data.startCell.col}, {"row", data.startCell.row}};
    j["start_direction"] = DirectionName(data.startDirection);
    j["food_seed"] = data.foodSeed;
    j["apple_count"] = data.appleCount;

    std::vector<std::string> directionNames;
    directionNames.reserve(data.directions.size());
    for (Direction d : data.directions) directionNames.push_back(DirectionName(d));
    j["directions"] = directionNames;

    std::ofstream out(path);
    if (!out) throw std::runtime_error("failed to open replay file for writing: " + path);
    out << j.dump(2);
}

ReplayData Load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("failed to open replay file: " + path);

    nlohmann::json j;
    try {
        in >> j;
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("failed to parse replay file: " + std::string(e.what()));
    }

    ReplayData data;
    data.startCell = Cell{j.at("start_cell").at("col").get<int>(), j.at("start_cell").at("row").get<int>()};
    data.startDirection = DirectionFromName(j.at("start_direction").get<std::string>());
    data.foodSeed = j.at("food_seed").get<unsigned int>();
    data.appleCount = j.at("apple_count").get<int>();
    for (const auto& name : j.at("directions")) {
        data.directions.push_back(DirectionFromName(name.get<std::string>()));
    }
    return data;
}

}  // namespace replay
