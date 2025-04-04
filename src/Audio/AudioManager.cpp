#include "AudioManager.h"
#include <iostream>
#include <spdlog/spdlog.h>
Engine::AudioManager* Engine::AudioManager::Instance = nullptr;

Engine::AudioManager::AudioManager()
{
    ma_engine_config engineConfig = ma_engine_config_init();
    if (ma_engine_init(&engineConfig, &Engine) != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize MiniAudio engine!" << std::endl;
    }
}

Engine::AudioManager::~AudioManager()
{
    for (auto& [id, sound] : Sounds)
    {
        ma_sound_uninit(sound.get());
    }
    Sounds.clear();

    ma_engine_uninit(&Engine);
}

Engine::AudioManager& Engine::AudioManager::GetInstance()
{
    if (!Instance)
    {
        Instance = new AudioManager();
    }
    return *Instance;
}

void Engine::AudioManager::DestroyInstance()
{
    if (Instance)
    {
        delete Instance;
        Instance = nullptr;
    }
}

std::unordered_map<std::string, std::shared_ptr<ma_sound>> Engine::AudioManager::GetSounds()
{
    return Sounds;
}

bool Engine::AudioManager::LoadSound(const std::string& Filename, const std::string& Id)
{
    ma_sound* newSound = new ma_sound;

    if (ma_sound_init_from_file(&Engine, Filename.c_str(), 0, nullptr, nullptr, newSound) != MA_SUCCESS)
    {
        std::cerr << "Failed to load audio file: " << Filename << std::endl;
        delete newSound;
        return false;
    }

    Sounds[Id] = std::shared_ptr<ma_sound>(newSound,
                                           [](ma_sound* Sound)
                                           {
                                               ma_sound_uninit(Sound);
                                               delete Sound;
                                           });
    return true;
}

void Engine::AudioManager::PlayAudio(const std::string& Id)
{
    auto it = Sounds.find(Id);
    if (it != Sounds.end())
    {
        if (!ma_sound_is_playing(it->second.get()))
        {
            ma_sound_start(it->second.get());
        }
    }
    else
    {
        std::cerr << "Sound with ID '" << Id << "' not found!" << std::endl;
    }
}

void Engine::AudioManager::SetVolumeGlobal(float Volume)
{
    ma_engine_set_volume(&Engine, Volume);
}

void Engine::AudioManager::SetVolume(const std::string& Id, float Volume)
{
    auto it = Sounds.find(Id);
    if (it != Sounds.end())
    {
        ma_sound_set_volume(it->second.get(), Volume);
    }
    else
    {
        std::cerr << "Sound with ID '" << Id << "' not found!" << std::endl;
    }
}

void Engine::AudioManager::PauseSound(const std::string& Id)
{
    auto it = Sounds.find(Id);
    if (it != Sounds.end())
    {
        ma_sound_stop(it->second.get());
    }
    else
    {
        std::cerr << "Sound with ID '" << Id << "' not found!" << std::endl;
    }
}

void Engine::AudioManager::StopSound(const std::string& Id)
{
    auto it = Sounds.find(Id);
    if (it != Sounds.end())
    {
        ma_sound_stop(it->second.get());
        ma_sound_seek_to_pcm_frame(it->second.get(), 0);
    }
    else
    {
        std::cerr << "Sound with ID '" << Id << "' not found!" << std::endl;
    }
}

void Engine::AudioManager::SetLooping(const std::string& Id, bool Loop)
{
    auto it = Sounds.find(Id);
    if (it != Sounds.end())
    {
        ma_sound_set_looping(it->second.get(), Loop);
    }
    else
    {
        std::cerr << "Sound with ID '" << Id << "' not found!" << std::endl;
    }
}

void Engine::AudioManager::SetListenerPosition(float X, float Y, float Z)
{
    ma_engine_listener_set_position(&Engine, 0, X, Y, Z);
}

void Engine::AudioManager::SetListenerOrientation(float ForwardX, float ForwardY, float ForwardZ, float UpX, float UpY,
                                                  float UpZ)
{
    ma_engine_listener_set_direction(&Engine, 0, ForwardX, ForwardY, ForwardZ);
    ma_engine_listener_set_world_up(&Engine, 0, UpX, UpY, UpZ);
}

void Engine::AudioManager::SetSoundPosition(const std::string& Id, float X, float Y, float Z)
{
    auto it = Sounds.find(Id);
    if (it != Sounds.end())
    {
        ma_sound_set_position(it->second.get(), X, Y, Z);
    }
    else
    {
        std::cerr << "Sound with ID '" << Id << "' not found!" << std::endl;
    }
}

void Engine::AudioManager::ConfigureSoundAttenuation(const std::string& Id, float MinDist, float MaxDist)
{
    auto it = Sounds.find(Id);
    if (it != Sounds.end())
    {
        ma_sound_set_attenuation_model(it->second.get(), ma_attenuation_model_exponential);
        ma_sound_set_min_distance(it->second.get(), MinDist);
        ma_sound_set_max_distance(it->second.get(), MaxDist);
        ma_sound_set_rolloff(it->second.get(), 2.0f);
    }
    else
    {
        std::cerr << "Sound with ID '" << Id << "' not found!" << std::endl;
    }
}
