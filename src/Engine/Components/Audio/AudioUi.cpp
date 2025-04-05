#include "AudioUi.h"
#include <filesystem>
#include "imgui.h"
#include "Engine/EngineObjects/Entity.h"
#include "Serialization/SerializationUtility.h"

Engine::AudioUi::AudioUi() :
    AudioManager(AudioManager::GetInstance())
{
}

void Engine::AudioUi::LoadSounds()
{
    std::string soundDir = "./res/sounds";

    for (const auto& entry : std::filesystem::directory_iterator(soundDir))
    {
        if (entry.is_regular_file())
        {
            std::string filePath = entry.path().string();
            std::string fileName = entry.path().filename().string();
            std::string fileId = fileName.substr(0, fileName.find_last_of('.'));

            AudioManager.LoadSound(filePath, fileId);

            if (SoundConfigs.find(fileId) == SoundConfigs.end())
            {
                SoundConfigs[fileId] = SoundSettings();
            }
        }
    }
}

void Engine::AudioUi::RenderUi()
{
    ImGui::Begin("Audio Settings");

    if (ImGui::BeginListBox("Loaded Sounds"))
    {
        for (const auto& sound : AudioManager.GetSounds())
        {
            bool isSelected = (SelectedSoundId == sound.first);
            if (ImGui::Selectable(sound.first.c_str(), isSelected))
            {
                SelectedSoundId = sound.first;
            }
        }
        ImGui::EndListBox();
    }

    if (!SelectedSoundId.empty())
    {
        SoundSettings& settings = SoundConfigs[SelectedSoundId];

        if (ImGui::Button("Play"))
        {
            AudioManager.PlayAudio(SelectedSoundId);
        }

        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            AudioManager.PauseSound(SelectedSoundId);
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            AudioManager.StopSound(SelectedSoundId);
        }

        if (ImGui::SliderFloat("Global Volume", &GlobalSoundVolume, 0.0f, 1.0f))
        {
            AudioManager.SetVolumeGlobal(GlobalSoundVolume);
        }

        if (ImGui::SliderFloat("Sound Volume", &settings.SoundVolume, 0.0f, 1.0f))
        {
            AudioManager.SetVolume(SelectedSoundId, settings.SoundVolume);
        }

        if (ImGui::Checkbox("Looping", &settings.Looping))
        {
            AudioManager.SetLooping(SelectedSoundId, settings.Looping);
        }

        if (ImGui::SliderFloat3("Sound Position", &settings.Position.X, -10.0f, 10.0f))
        {
            AudioManager.SetSoundPosition(SelectedSoundId, settings.Position.X, settings.Position.Y,
                                          settings.Position.Z);
            AudioManager.ConfigureSoundAttenuation(SelectedSoundId, 1, 30);
        }
    }

    ImGui::End();
}

rapidjson::Value Engine::AudioUi::Serialize(rapidjson::Document::AllocatorType& Allocator) const
{
    START_COMPONENT_SERIALIZATION
    SERIALIZE_FIELD(GlobalSoundVolume)
    for (auto& soundConfig : SoundConfigs)
    {
        SoundSettings settings = soundConfig.second;

        SERIALIZE_FIELD(settings.SoundId)
        SERIALIZE_FIELD(settings.SoundVolume)
        SERIALIZE_FIELD(settings.Looping)
        SERIALIZE_FIELD(settings.Position.X)
        SERIALIZE_FIELD(settings.Position.Y)
        SERIALIZE_FIELD(settings.Position.Z)
    }
    END_COMPONENT_SERIALIZATION
}

void Engine::AudioUi::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_VALUE_PASS
    DESERIALIZE_VALUE(GlobalSoundVolume)
    for (auto& soundConfig : SoundConfigs)
    {
        SoundSettings settings = soundConfig.second;

        DESERIALIZE_VALUE(settings.SoundId)
        DESERIALIZE_VALUE(settings.SoundVolume)
        DESERIALIZE_VALUE(settings.Looping)
        DESERIALIZE_VALUE(settings.Position.X)
        DESERIALIZE_VALUE(settings.Position.Y)
        DESERIALIZE_VALUE(settings.Position.Z)
    }
    END_COMPONENT_DESERIALIZATION_VALUE_PASS
}

void Engine::AudioUi::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
}
