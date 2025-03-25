#include "AudioManager.h"
#include <iostream>

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager()
{
    if (ma_engine_init(nullptr, &engine) != MA_SUCCESS) {
        std::cerr << "Failed to initialize MiniAudio engine!" << std::endl;
    }
}

AudioManager::~AudioManager()
{ ma_engine_uninit(&engine); }

AudioManager& AudioManager::getInstance()
{
    if (!instance) {
        instance = new AudioManager();
    }
    return *instance;
}

void AudioManager::destroyInstance()
{
    delete instance;
    instance = nullptr;
}

bool AudioManager::loadSound(const std::string& filename, const std::string& id)
{
    ma_sound* newSound = new ma_sound;

    if (ma_sound_init_from_file(&engine, filename.c_str(), 0, nullptr, nullptr, newSound) != MA_SUCCESS) {
        std::cerr << "Failed to load audio file: " << filename << std::endl;
        delete newSound;
        return false;
    }

    sounds[id] = std::shared_ptr<ma_sound>(newSound,
                                           [](ma_sound* sound)
                                           {
                                               ma_sound_uninit(sound);
                                               delete sound;
                                           });

    return true;
}

void AudioManager::playSound(const std::string& id)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        ma_sound_start(it->second.get());
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}

void AudioManager::setVolume(float volume)
{ ma_engine_set_volume(&engine, volume); }

void AudioManager::stopSound(const std::string& id)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        ma_sound_stop(it->second.get());
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}
