#include "NavArea.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    void NavArea::SetWalkable(bool Value)
    {
        IsWalkable = Value;
    }

    [[nodiscard]] bool NavArea::GetWalkable() const
    {
        return IsWalkable;
    }

#if EDITOR
    void NavArea::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Navigation Area", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("IsWalkable", &IsWalkable);
        }
    }
#endif

    rapidjson::Value NavArea::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(IsWalkable)
        END_COMPONENT_SERIALIZATION
    }

    void NavArea::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(IsWalkable)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void NavArea::DeserializeReferencesPass(const rapidjson::Value& Object,
                                            Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

}
