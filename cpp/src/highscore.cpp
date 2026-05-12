#include "highscore.hpp"

#include <fstream>

#include "json.hpp"

HighScoreStore::HighScoreStore(std::string path) : path_(std::move(path)), value_(Load()) {}

int HighScoreStore::Load() const {
    std::ifstream in(path_);
    if (!in) return 0;
    try {
        nlohmann::json j;
        in >> j;
        return j.value("high_score", 0);
    } catch (const nlohmann::json::exception&) {
        return 0;
    }
}

bool HighScoreStore::Update(int score) {
    if (score <= value_) return false;
    value_ = score;
    Save();
    return true;
}

void HighScoreStore::Save() const {
    std::ofstream out(path_);
    if (!out) return;
    nlohmann::json j;
    j["high_score"] = value_;
    out << j.dump();
}
