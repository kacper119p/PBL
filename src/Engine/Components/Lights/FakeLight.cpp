#include "FakeLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    FakeLight::FakeLight() = default;

    FakeLight::~FakeLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
    }

    void FakeLight::Start()
    {
        Component::Start();
        LightManager::GetInstance()->RegisterLight(this);
    }
#if EDITOR
    void FakeLight::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Fake Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
        }
    }
#endif
    rapidjson::Value FakeLight::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void FakeLight::DeserializeValuePass(const rapidjson::Value& Object,
                                         Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void FakeLight::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
