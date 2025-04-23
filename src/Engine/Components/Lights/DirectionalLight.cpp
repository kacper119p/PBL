#include "DirectionalLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    DirectionalLight::DirectionalLight() = default;

    DirectionalLight::~DirectionalLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
#if EDITOR
        LightsGui::UnregisterLight(this);
#endif
    }

    void DirectionalLight::Start()
    {
        Component::Start();
        LightManager::GetInstance()->RegisterLight(this);
#if EDITOR
        LightsGui::RegisterLight(this);
#endif
    }
#if EDITOR
    void DirectionalLight::DrawImGui() 
    {
        if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float color[3] = {Color.r, Color.g, Color.b};
            if (ImGui::ColorEdit3("Color", color))
            {
                SetColor(glm::vec3(color[0], color[1], color[2]));
            }
        }
    }
    #endif
    rapidjson::Value DirectionalLight::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Color)
        END_COMPONENT_SERIALIZATION
    }

    void DirectionalLight::DeserializeValuePass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Color)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void DirectionalLight::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                     Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
