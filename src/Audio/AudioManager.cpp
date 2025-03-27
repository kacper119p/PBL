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
        if (!ma_sound_is_playing(it->second.get())) {
            ma_sound_start(it->second.get());
        }
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}

void AudioManager::setVolumeGlobal(float volume)
{ ma_engine_set_volume(&engine, volume); }

void AudioManager::setVolume(const std::string& id, float volume)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        ma_sound_set_volume(it->second.get(), volume);
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}

void AudioManager::pauseSound(const std::string& id)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        ma_sound_stop(it->second.get());
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}

void AudioManager::stopSound(const std::string& id)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        ma_sound_stop(it->second.get());
        ma_sound_seek_to_pcm_frame(it->second.get(), 0);
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}

void AudioManager::setLooping(const std::string& id, bool loop)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        ma_sound_set_looping(it->second.get(), loop);
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}

void AudioManager::setListenerPosition(float x, float y, float z)
{ ma_engine_listener_set_position(&engine, 0, x, y, z); }

void AudioManager::setListenerOrientation(float forwardX, float forwardY, float forwardZ,
                                          float upX, float upY, float upZ)
{
    ma_engine_listener_set_direction(&engine, 0, forwardX, forwardY, forwardZ);
    ma_engine_listener_set_world_up(&engine, 0, upX, upY, upZ);
}

void AudioManager::setSoundPosition(const std::string& id, float x, float y, float z)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        ma_sound_set_position(it->second.get(), x, y, z);
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}

void AudioManager::configureSoundAttenuation(const std::string& id, float minDist, float maxDist)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        ma_sound_set_attenuation_model(it->second.get(), ma_attenuation_model_exponential);
        ma_sound_set_min_distance(it->second.get(), minDist);
        ma_sound_set_max_distance(it->second.get(), maxDist);
        ma_sound_set_rolloff(it->second.get(), 1.2f);
    } else {
        std::cerr << "Sound with ID '" << id << "' not found!" << std::endl;
    }
}
