#include <cstdio>
#include <fstream>
#include <string>

#include "doctest.h"
#include "highscore.hpp"

namespace {

// unique scratch path per test run
std::string TempPath() {
    static int counter = 0;
    return "test_highscore_" + std::to_string(counter++) + ".json";
}

}  // namespace

TEST_CASE("defaults to zero when no file exists") {
    std::string path = TempPath();
    std::remove(path.c_str());
    HighScoreStore store(path);
    CHECK(store.Value() == 0);
}

TEST_CASE("update persists a new high score to disk") {
    std::string path = TempPath();
    std::remove(path.c_str());
    HighScoreStore store(path);
    store.Update(12);

    std::ifstream in(path);
    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    CHECK(contents.find("12") != std::string::npos);
    std::remove(path.c_str());
}

TEST_CASE("update ignores a lower score") {
    std::string path = TempPath();
    std::remove(path.c_str());
    HighScoreStore store(path);
    store.Update(12);
    store.Update(5);
    CHECK(store.Value() == 12);
    std::remove(path.c_str());
}

TEST_CASE("a new store reloads the persisted value") {
    std::string path = TempPath();
    std::remove(path.c_str());
    HighScoreStore first(path);
    first.Update(9);

    HighScoreStore reloaded(path);
    CHECK(reloaded.Value() == 9);
    std::remove(path.c_str());
}
