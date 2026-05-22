#include "audio_manager.hpp"

AudioManager::AudioManager() {
    InitAudioDevice();
    // no audio device (headless/CI), fail soft
    enabled_ = IsAudioDeviceReady();
    if (enabled_) {
        eat_ = LoadSound("assets/eating sound effect.wav");
        death_ = LoadSound("assets/death sound.wav");
        enabled_ = eat_.frameCount > 0 && death_.frameCount > 0;
    }
}

AudioManager::~AudioManager() {
    if (enabled_) {
        UnloadSound(eat_);
        UnloadSound(death_);
    }
    if (IsAudioDeviceReady()) CloseAudioDevice();
}

void AudioManager::PlayEat() const {
    if (enabled_) PlaySound(eat_);
}

void AudioManager::PlayDeath() const {
    if (enabled_) PlaySound(death_);
}
