//#if EDITOR
#include "AudioUi.h"
#include <filesystem>
#include "imgui.h"
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
        }
    }
}

void Engine::AudioUi::RenderUi()
{
    ImGui::Begin("Audio Settings");

    static std::string selectedSoundId;

    if (ImGui::BeginListBox("Loaded Sounds"))
    {
        for (const auto& sound : AudioManager.GetSounds())
        {
            bool isSelected = (selectedSoundId == sound.first);
            if (ImGui::Selectable(sound.first.c_str(), isSelected))
            {
                selectedSoundId = sound.first;
            }
        }
        ImGui::EndListBox();
    }

    if (!selectedSoundId.empty())
    {
        if (ImGui::Button("Play"))
        {
            AudioManager.PlayAudio(selectedSoundId);
        }

        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            AudioManager.PauseSound(selectedSoundId);
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            AudioManager.StopSound(selectedSoundId);
        }

        if (ImGui::SliderFloat("Global Volume", &GlobalSoundVolume, 0.0f, 1.0f))
        {
            AudioManager.SetVolumeGlobal(GlobalSoundVolume);
        }

        if (ImGui::SliderFloat("Sound Volume", &SoundVolume, 0.0f, 1.0f))
        {
            AudioManager.SetVolume(selectedSoundId, SoundVolume);
        }

        if (ImGui::Checkbox("Looping", &Looping))
        {
            AudioManager.SetLooping(selectedSoundId, Looping);
        }

        if (ImGui::SliderFloat3("Sound Position", SoundPos, -10.0f, 10.0f))
        {
            AudioManager.SetSoundPosition(selectedSoundId, SoundPos[0], SoundPos[1], SoundPos[2]);
            AudioManager.ConfigureSoundAttenuation(selectedSoundId, 1, 30);
        }
    }

    ImGui::End();
}


//#endif
