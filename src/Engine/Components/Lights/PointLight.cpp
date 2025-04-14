#include "PointLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    void PointLight::Start()
    {
        Component::Start();
        LightManager::GetInstance()->RegisterLight(this);
#if EDITOR
        LightsGui::RegisterLight(this);
#endif
    }

    void PointLight::DrawImGui()
    {
        if (ImGui::CollapsingHeader("PointLight", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Display and edit the Color (glm::vec3)
            float color[3] = {Color.r, Color.g, Color.b};
            if (ImGui::ColorEdit3("Color", color))
            {
                SetColor(glm::vec3(color[0], color[1], color[2]));
            }

            // Display and edit the Linear Falloff
            float linearFalloff = GetLinearFalloff();
            if (ImGui::DragFloat("Linear Falloff", &linearFalloff, 0.01f, 0.0f, 10.0f))
            {
                SetLinearFalloff(linearFalloff);
            }

            // Display and edit the Quadratic Falloff
            float quadraticFalloff = GetQuadraticFalloff();
            if (ImGui::DragFloat("Quadratic Falloff", &quadraticFalloff, 0.01f, 0.0f, 10.0f))
            {
                SetQuadraticFalloff(quadraticFalloff);
            }

            // Display and edit the Range
            float range = GetRange();
            if (ImGui::DragFloat("Range", &range, 1.0f, 0.0f, 1000.0f))
            {
                SetRange(range);
            }
        }
    }

    PointLight::~PointLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
#if EDITOR
        LightsGui::UnregisterLight(this);
#endif
    }

    rapidjson::Value PointLight::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Color)
        SERIALIZE_FIELD(LinearFalloff)
        SERIALIZE_FIELD(QuadraticFalloff)
        SERIALIZE_FIELD(Range)
        END_COMPONENT_SERIALIZATION
    }

    void PointLight::DeserializeValuePass(const rapidjson::Value& Object,
                                          Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Color)
        DESERIALIZE_VALUE(LinearFalloff)
        DESERIALIZE_VALUE(QuadraticFalloff)
        DESERIALIZE_VALUE(Range)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void PointLight::DeserializeReferencesPass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
