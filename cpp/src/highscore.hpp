// persists the high score to a json file on disk
#pragma once

#include <string>

class HighScoreStore {
public:
    explicit HighScoreStore(std::string path);

    int Value() const { return value_; }

    // true and persists if score beats the previous best
    bool Update(int score);

private:
    std::string path_;
    int value_;

    int Load() const;
    void Save() const;
};
