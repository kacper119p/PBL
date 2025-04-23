#include "Rotator.h"

#include "Serialization/SerializationUtility.h"

namespace Engine
{
    void Rotator::Update(float DeltaTime)
    {
        GetOwner()->GetTransform()->SetEulerAngles(GetOwner()->GetTransform()->GetEulerAngles() + Velocity * DeltaTime);
    }
#if EDITOR
    void Rotator::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Ship Rotator"))
        {
            ImGui::InputFloat3("Velocity##Rotator", glm::value_ptr(Velocity));
        }
    }
#endif
    rapidjson::Value Rotator::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Velocity)
        END_COMPONENT_SERIALIZATION
    }

    void Rotator::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Velocity)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Rotator::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
