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
        for (size_t i = 0; i < AudioManager.GetLoadedSounds().size(); ++i)
        {
            ma_sound* sound = AudioManager.GetLoadedSounds()[i].get();
            bool isSelected = (SelectedSound == sound);
            if (ImGui::Selectable(std::to_string(i).c_str(), isSelected))
            {
                SelectedSound = sound;
                AudioManager.SetSoundPosition(*SelectedSound, GetOwner()->GetTransform()->GetPosition());
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

    ImGui::End();
}

void Engine::AudioSource::SelectEntityForAudioControl(Entity* Entity)
{
    if (Entity != nullptr)
    {
        IsEntitySelected = true;
        AudioManager.SetSoundPosition(*SelectedSound, Entity->GetTransform()->GetPosition());
    }
    else
    {
        IsEntitySelected = false;
    }
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
