#include "MoveForward.h"

#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    void MoveForward::Update(const float DeltaTime)
    {
        Transform* transform = GetOwner()->GetTransform();
        transform->SetPosition(transform->GetPosition() + transform->GetForward() * Speed * DeltaTime);
    }
#if EDITOR
    void MoveForward::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Move Forward"))
        {
            ImGui::InputFloat("Speed##MoveForward", &Speed);
        }
    }
#endif
    rapidjson::Value MoveForward::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Speed)
        END_COMPONENT_SERIALIZATION
    }

    void MoveForward::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Speed)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void MoveForward::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
