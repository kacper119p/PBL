#include "AudioManager.h"
#include <iostream>

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() {
    if (soloud.init() != 0) {
        std::cerr << "Failed to initialize SoLoud!" << std::endl;
    }
}

AudioManager::~AudioManager() {
    soloud.deinit();
}

AudioManager& AudioManager::getInstance() {
    if (!instance) {
        instance = new AudioManager();
    }
    return *instance;
}

void AudioManager::destroyInstance() {
    delete instance;
    instance = nullptr;
}

bool AudioManager::loadSound(const std::string& filename) {
    if (currentSound.load(filename.c_str()) != 0) {
        std::cerr << "Failed to load audio file: " << filename << std::endl;
        return false;
    }
    return true;
}

void AudioManager::playSound() {
    soloud.play(currentSound);
}

void AudioManager::setVolume(float volume) {
    soloud.setGlobalVolume(volume);
}

void AudioManager::pauseSound() {
    soloud.setPauseAll(true);
}

void AudioManager::resumeSound() {
    soloud.setPauseAll(false);
}

void AudioManager::stopSound() {
    soloud.stopAll();
}
