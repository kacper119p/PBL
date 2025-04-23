#include "SpotLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    void SpotLight::Start()
    {
        Component::Start();
        LightManager::GetInstance()->RegisterLight(this);
#if EDITOR
        LightsGui::RegisterLight(this);
#endif
    }
#if EDITOR
    void SpotLight::DrawImGui() {
        if (ImGui::CollapsingHeader("SpotLight", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Editable Color (with color picker)
            if (ImGui::ColorEdit3("Color", glm::value_ptr(Color)))
            {
                SetColor(Color);
            }

            // Editable Inner Angle
            if (ImGui::DragFloat("Inner Angle", &InnerAngle, 1.0f, 0.0f, OuterAngle))
            {
                SetInnerAngle(InnerAngle);
            }

            // Editable Outer Angle
            if (ImGui::DragFloat("Outer Angle", &OuterAngle, 1.0f, InnerAngle, 180.0f))
            {
                SetOuterAngle(OuterAngle);
            }

            // Editable Linear Falloff
            if (ImGui::DragFloat("Linear Falloff", &LinearFalloff, 0.01f, 0.0f, 10.0f))
            {
                SetLinearFalloff(LinearFalloff);
            }

            // Editable Quadratic Falloff
            if (ImGui::DragFloat("Quadratic Falloff", &QuadraticFalloff, 0.01f, 0.0f, 10.0f))
            {
                SetQuadraticFalloff(QuadraticFalloff);
            }

            // Editable Range
            if (ImGui::DragFloat("Range", &Range, 1.0f, 0.0f, 1000.0f))
            {
                SetRange(Range);
            }
        }
    }
    #endif
    SpotLight::~SpotLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
#if EDITOR
        LightsGui::UnregisterLight(this);
#endif
    }

    rapidjson::Value SpotLight::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Color)
        SERIALIZE_FIELD(OuterAngle)
        SERIALIZE_FIELD(InnerAngle)
        SERIALIZE_FIELD(LinearFalloff)
        SERIALIZE_FIELD(QuadraticFalloff)
        SERIALIZE_FIELD(Range)
        END_COMPONENT_SERIALIZATION
    }

    void SpotLight::DeserializeValuePass(const rapidjson::Value& Object,
                                         Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Color)
        DESERIALIZE_VALUE(OuterAngle)
        DESERIALIZE_VALUE(InnerAngle)
        DESERIALIZE_VALUE(LinearFalloff)
        DESERIALIZE_VALUE(QuadraticFalloff)
        DESERIALIZE_VALUE(Range)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void SpotLight::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
