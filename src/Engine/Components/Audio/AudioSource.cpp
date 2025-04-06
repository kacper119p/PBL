#include "AudioSource.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Entity.h"
#include "imgui.h"

Engine::AudioSource::AudioSource() :
    AudioManager(AudioManager::GetInstance()), SelectedSound(nullptr), IsEntitySelected(false)
{
}

void Engine::AudioSource::RenderAudioSourceImGui()
{
    if (!IsEntitySelected)
    {
        ImGui::Text("Please select an entity to configure audio.");
        return;
    }

    ImGui::Begin("Audio Settings");

    if (ImGui::BeginListBox("Loaded Sounds"))
    {
        bool isNoneSelected = (SelectedSound == nullptr);
        if (ImGui::Selectable("None", isNoneSelected))
        {
            if (SelectedSound)
            {
                AudioManager.StopSound(*SelectedSound);
                ResetAudioSettings();
            }
            SelectedSound = nullptr;
        }

        for (const auto& soundPair : AudioManager.GetLoadedSounds())
        {
            std::string soundId = soundPair.first;
            ma_sound* sound = soundPair.second.get();
            bool isSelected = (SelectedSound == sound);

            if (ImGui::Selectable(soundId.c_str(), isSelected))
            {
                if (SelectedSound)
                {
                    AudioManager.StopSound(*SelectedSound);
                    ResetAudioSettings();
                }

                SelectedSound = sound;

                ResetAudioSettings();

                AudioManager.SetSoundPosition(*SelectedSound, this->GetOwner()->GetTransform()->GetPosition());
            }
        }

        ImGui::EndListBox();
    }

    if (SelectedSound)
    {
        if (ImGui::Button("Play"))
        {
            AudioManager.PlayAudio(*SelectedSound);
        }

        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            AudioManager.PauseSound(*SelectedSound);
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            AudioManager.StopSound(*SelectedSound);
        }

        if (ImGui::SliderFloat("Sound Volume", &SoundVolume, 0.0f, 1.0f))
        {
            AudioManager.SetVolume(*SelectedSound, SoundVolume);
        }

        if (ImGui::Checkbox("Looping", &Looping))
        {
            AudioManager.SetLooping(*SelectedSound, Looping);
        }

        if (ImGui::SliderFloat("Attenuation Min Distance", &MinDist, 0.1f, 100.0f))
        {
            AudioManager.ConfigureSoundAttenuation(*SelectedSound, MinDist, MaxDist, RollOff);
        }

        if (ImGui::SliderFloat("Attenuation Max Distance", &MaxDist, 1.0f, 500.0f))
        {
            AudioManager.ConfigureSoundAttenuation(*SelectedSound, MinDist, MaxDist, RollOff);
        }

        if (ImGui::SliderFloat("Attenuation Roll Off", &RollOff, 0.0f, 10.0f))
        {
            AudioManager.ConfigureSoundAttenuation(*SelectedSound, MinDist, MaxDist, RollOff);
        }
    }
    else
    {
        ImGui::Text("No sound selected.");
    }

    ImGui::End();
}

void Engine::AudioSource::SelectEntityForAudioControl(Entity* Entity)
{
    if (Entity != nullptr)
    {
        IsEntitySelected = true;
    }
    else
    {
        IsEntitySelected = false;
    }
}

void Engine::AudioSource::ResetAudioSettings()
{
    SoundVolume = 1.0f;
    Looping = false;
    MinDist = 1.0f;
    MaxDist = 100.0f;
    RollOff = 1.0f;
}

rapidjson::Value Engine::AudioSource::Serialize(rapidjson::Document::AllocatorType& Allocator) const
{
    START_COMPONENT_SERIALIZATION
    SERIALIZE_FIELD(SoundVolume)
    SERIALIZE_FIELD(Looping)
    SERIALIZE_FIELD(Position.x)
    SERIALIZE_FIELD(Position.y)
    SERIALIZE_FIELD(Position.z)
    SERIALIZE_FIELD(MinDist)
    SERIALIZE_FIELD(MaxDist)
    SERIALIZE_FIELD(RollOff)
    END_COMPONENT_SERIALIZATION
}

void Engine::AudioSource::DeserializeValuePass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_VALUE_PASS
    DESERIALIZE_VALUE(SoundVolume)
    DESERIALIZE_VALUE(Looping)
    DESERIALIZE_VALUE(Position.x)
    DESERIALIZE_VALUE(Position.y)
    DESERIALIZE_VALUE(Position.z)
    DESERIALIZE_VALUE(MinDist)
    DESERIALIZE_VALUE(MaxDist)
    DESERIALIZE_VALUE(RollOff)
    END_COMPONENT_DESERIALIZATION_VALUE_PASS
}

void Engine::AudioSource::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                    Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
}
