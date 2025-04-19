#include "AudioManager.h"
#include "spdlog/spdlog.h"
#include "imgui.h"

Engine::AudioManager* Engine::AudioManager::Instance = nullptr;

Engine::AudioManager::AudioManager()
{
    if (ma_engine_init(nullptr, &Engine) != MA_SUCCESS)
    {
        spdlog::error("Failed to initialize MiniAudio engine!");
    }

    LoadSounds();
}

Engine::AudioManager::~AudioManager()
{
    for (auto& sound : Sounds)
    {
        ma_sound_uninit(sound.second.get());
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

bool Engine::AudioManager::LoadSound(const std::string& Filename, ma_sound& Sound)
{
    ma_result result = ma_sound_init_from_file(&Engine, Filename.c_str(), 0, nullptr, nullptr, &Sound);
    if (result != MA_SUCCESS)
    {
        spdlog::error("Failed to load audio file: {}", Filename);
        return false;
    }
    return true;
}

std::map<std::string, std::shared_ptr<ma_sound>> Engine::AudioManager::GetLoadedSounds() const
{
    return Sounds;
}

void Engine::AudioManager::LoadSounds()
{
    std::string soundDir = "./res/sounds";

    for (const auto& entry : std::filesystem::directory_iterator(soundDir))
    {
        if (entry.is_regular_file())
        {
            std::string filePath = entry.path().string();
            std::string fileId = entry.path().stem().string();

            auto sound = std::make_shared<ma_sound>();
            if (LoadSound(filePath, *sound))
            {
                Sounds[fileId] = sound;
            }
        }
    }
}

void Engine::AudioManager::PlayAudio(ma_sound& Sound)
{
    if (!ma_sound_is_playing(&Sound))
    {
        ma_sound_start(&Sound);
    }
}

void Engine::AudioManager::SetGlobalVolume(float Volume)
{
    ma_engine_set_volume(&Engine, Volume);
}

float Engine::AudioManager::GetGlobalVolume()
{
    return ma_engine_get_volume(&Engine);
}

void Engine::AudioManager::SetVolume(ma_sound& Sound, float Volume)
{
    ma_sound_set_volume(&Sound, Volume);
}

void Engine::AudioManager::PauseSound(ma_sound& Sound)
{
    ma_sound_stop(&Sound);
}

void Engine::AudioManager::StopSound(ma_sound& Sound)
{
    ma_sound_stop(&Sound);
    ma_sound_seek_to_pcm_frame(&Sound, 0);
}

void Engine::AudioManager::SetLooping(ma_sound& Sound, bool Loop)
{
    ma_sound_set_looping(&Sound, Loop);
}

void Engine::AudioManager::SetListenerPosition(const glm::vec3& Position)
{
    ma_engine_listener_set_position(&Engine, 0, Position.x, Position.y, Position.z);
}

void Engine::AudioManager::SetListenerOrientation(glm::vec3 Forward, glm::vec3 Up)
{
    ma_engine_listener_set_direction(&Engine, 0, Forward.x, Forward.y, Forward.z);
    ma_engine_listener_set_world_up(&Engine, 0, Up.x, Up.y, Up.z);
}

void Engine::AudioManager::SetSoundPosition(ma_sound& Sound, const glm::vec3& Position)
{
    ma_sound_set_position(&Sound, Position.x, Position.y, Position.z);
}

void Engine::AudioManager::ConfigureSoundAttenuation(ma_sound& Sound, float MinDist, float MaxDist, float RollOff)
{
    ma_sound_set_attenuation_model(&Sound, ma_attenuation_model_exponential);
    ma_sound_set_min_distance(&Sound, MinDist);
    ma_sound_set_max_distance(&Sound, MaxDist);
    ma_sound_set_rolloff(&Sound, RollOff);
}

#if EDITOR
void Engine::AudioManager::RenderGlobalVolumeImGui()
{
    ImGui::Begin("Engine Properties");
    if (ImGui::CollapsingHeader("Audio Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        float globalVolume = GetInstance().GetGlobalVolume();
        if (ImGui::SliderFloat("Global Volume", &globalVolume, 0.0f, 1.0f))
        {
            GetInstance().SetGlobalVolume(globalVolume);
        }
    }

    ImGui::End();
}
#endif
