#include "AudioSource.h"
#include "spdlog/spdlog.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/UpdateManager.h"

Engine::AudioSource::AudioSource() :
    AudioManager(AudioManager::GetInstance())
{
    UpdateManager::GetInstance()->RegisterComponent(this);
}

Engine::AudioSource::~AudioSource()
{
    SoundInstance.reset();
    UpdateManager::GetInstance()->UnregisterComponent(this);
}

void Engine::AudioSource::Start()
{
    if (!SelectedSoundId.empty() && !SoundInstance && !EDITOR)
    {
        SoundInstance = AudioManager.CreateSoundInstance(SelectedSoundId);
        AudioManager.PlayAudio(SoundInstance);
        AudioManager.SetLooping(SoundInstance, Looping);
        AudioManager.SetVolume(SoundInstance, SoundVolume);
        AudioManager.SetSoundPosition(SoundInstance, GetOwner()->GetTransform()->GetPosition());
        AudioManager.ConfigureSoundAttenuation(SoundInstance, MinDist, MaxDist, RollOff);
    }
}

void Engine::AudioSource::Update(float DeltaTime)
{
    if (!EDITOR)
        AudioManager.SetSoundPosition(SoundInstance, GetOwner()->GetTransform()->GetPosition());
}

std::shared_ptr<ma_sound> Engine::AudioSource::GetSoundInstance()
{
    return SoundInstance;
}

void Engine::AudioSource::ResetAudioSettings()
{
    SoundVolume = 1.0f;
    Looping = false;
    MinDist = 1.0f;
    MaxDist = 100.0f;
    RollOff = 1.0f;
}

#if EDITOR
void Engine::AudioSource::DrawImGui()
{
    if (ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginCombo("##sound_combo", SelectedSoundId.empty() ? "Select sound..." : SelectedSoundId.c_str()))
        {
            if (ImGui::Selectable("None", SelectedSoundId.empty()))
            {
                if (SoundInstance)
                {
                    AudioManager.StopSound(SoundInstance);
                    SoundInstance.reset();
                    ResetAudioSettings();
                }
                SelectedSoundId.clear();
            }

            for (const auto& soundPair : AudioManager.GetLoadedSounds())
            {
                std::string soundId = soundPair.first;
                bool isSelected = (SelectedSoundId == soundId);

                if (ImGui::Selectable(soundId.c_str(), isSelected))
                {
                    if (SoundInstance)
                    {
                        AudioManager.StopSound(SoundInstance);
                        SoundInstance.reset();
                    }

                    SoundInstance = AudioManager.CreateSoundInstance(soundId);
                    if (SoundInstance)
                    {
                        SelectedSoundId = soundId;
                        ResetAudioSettings();
                    }
                    else
                    {
                        SelectedSoundId.clear();
                    }
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (SoundInstance)
        {
            if (ImGui::Button("Play"))
            {
                AudioManager.PlayAudio(SoundInstance);
            }

            ImGui::SameLine();
            if (ImGui::Button("Pause"))
            {
                AudioManager.PauseSound(SoundInstance);
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop"))
            {
                AudioManager.StopSound(SoundInstance);
            }

            if (ImGui::SliderFloat("Sound Volume", &SoundVolume, 0.0f, 1.0f))
            {
                AudioManager.SetVolume(SoundInstance, SoundVolume);
            }

            if (ImGui::Checkbox("Looping", &Looping))
            {
                AudioManager.SetLooping(SoundInstance, Looping);
            }

            if (ImGui::SliderFloat("Attenuation Min Distance", &MinDist, 0.1f, 100.0f))
            {
                AudioManager.ConfigureSoundAttenuation(SoundInstance, MinDist, MaxDist, RollOff);
            }

            if (ImGui::SliderFloat("Attenuation Max Distance", &MaxDist, 1.0f, 500.0f))
            {
                AudioManager.ConfigureSoundAttenuation(SoundInstance, MinDist, MaxDist, RollOff);
            }

            if (ImGui::SliderFloat("Attenuation Roll Off", &RollOff, 0.0f, 10.0f))
            {
                AudioManager.ConfigureSoundAttenuation(SoundInstance, MinDist, MaxDist, RollOff);
            }

            AudioManager.SetSoundPosition(SoundInstance, this->GetOwner()->GetTransform()->GetPosition());
        }
        else
        {
            ImGui::Text("No sound selected.");
        }
    }
}
#endif

rapidjson::Value Engine::AudioSource::Serialize(rapidjson::Document::AllocatorType& Allocator) const
{
    START_COMPONENT_SERIALIZATION
    SERIALIZE_FIELD(SelectedSoundId)
    SERIALIZE_FIELD(SoundVolume)
    SERIALIZE_FIELD(Looping)
    SERIALIZE_FIELD(MinDist)
    SERIALIZE_FIELD(MaxDist)
    SERIALIZE_FIELD(RollOff)
    END_COMPONENT_SERIALIZATION
}

void Engine::AudioSource::DeserializeValuePass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_VALUE_PASS
    DESERIALIZE_VALUE(SelectedSoundId)
    DESERIALIZE_VALUE(SoundVolume)
    DESERIALIZE_VALUE(Looping)
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
