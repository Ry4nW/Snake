#pragma once

#include <raylib.h>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void PlayEat() const;
    void PlayDeath() const;

private:
    bool enabled_;
    Sound eat_{};
    Sound death_{};
};
