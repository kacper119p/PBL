#include "Thrash.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Scene/Scene.h"

namespace Engine
{
    
    void Thrash::Start() { 
        collider = GetOwner()->GetComponent<Collider>();
        collider->OnCollisionAddListener(ThrowOut);
    }

    rapidjson::Value Thrash::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(size)
        END_COMPONENT_SERIALIZATION
    }

    void Thrash::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(size)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Thrash::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }


    void Thrash::DeleteThrash(Engine::Collider* collider)
    {
        if (collider->GetOwner()->GetName() == "ThrashCan")
        {
            GetOwner()->GetScene()->DeleteEntity(GetOwner());
        }
    }
#if EDITOR
    void Thrash::DrawImGui()
    {
        static const char* sizeLabels[] = {"Small", "Medium", "Large"};
        int currentSizeIndex = 0;

        switch (size)
        {
            case ThrashSize::Small:
                currentSizeIndex = 0;
                break;
            case ThrashSize::Medium:
                currentSizeIndex = 1;
                break;
            case ThrashSize::Large:
                currentSizeIndex = 2;
                break;
        }

        if (ImGui::Combo("Size", &currentSizeIndex, sizeLabels, IM_ARRAYSIZE(sizeLabels)))
        {
            switch (currentSizeIndex)
            {
                case 0:
                    size = ThrashSize::Small;
                    break;
                case 1:
                    size = ThrashSize::Medium;
                    break;
                case 2:
                    size = ThrashSize::Large;
                    break;
            }
        }
    }
#endif
} // namespace Engine
