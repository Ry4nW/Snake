#include <cstdio>
#include <string>

#include "doctest.h"
#include "replay.hpp"

namespace {

std::string TempPath() {
    static int counter = 0;
    return "test_replay_" + std::to_string(counter++) + ".json";
}

}  // namespace

TEST_CASE("save then load round-trips every field exactly") {
    std::string path = TempPath();
    ReplayData original;
    original.startCell = Cell{3, 5};
    original.startDirection = Direction::Right;
    original.foodSeed = 123456789u;
    original.appleCount = 3;
    original.directions = {Direction::Right, Direction::Right, Direction::Down,
                            Direction::Left,  Direction::Up,    Direction::Up};

    replay::Save(path, original);
    ReplayData loaded = replay::Load(path);

    CHECK(loaded.startCell == original.startCell);
    CHECK(loaded.startDirection == original.startDirection);
    CHECK(loaded.foodSeed == original.foodSeed);
    CHECK(loaded.appleCount == original.appleCount);
    CHECK(loaded.directions == original.directions);

    std::remove(path.c_str());
}

TEST_CASE("round-trips an empty direction sequence") {
    std::string path = TempPath();
    ReplayData original;
    original.startCell = Cell{0, 0};
    original.foodSeed = 1;
    original.appleCount = 1;

    replay::Save(path, original);
    ReplayData loaded = replay::Load(path);

    CHECK(loaded.directions.empty());

    std::remove(path.c_str());
}

TEST_CASE("load throws on a missing file") {
    CHECK_THROWS_AS(replay::Load("this_file_does_not_exist.json"), std::runtime_error);
}
