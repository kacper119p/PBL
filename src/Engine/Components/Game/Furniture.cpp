#include "Furniture.h"
#include "ThrashManager.h"
#include "Engine/EngineObjects/Scene/Scene.h"
namespace Engine
{

    void Furniture::Start() 
    {
        collider = GetOwner()->GetComponent<Collider>();
        collider->OnCollisionAddListener(ThrowOut);
        ThrashManager::GetInstance()->AddFurniture(this);
    }

    void Furniture::OnDestroy() 
    { 
        if (collider)
        {
            collider->OnCollisionRemoveListener(ThrowOut);
        }
        ThrashManager::GetInstance()->RemoveFurniture(this);
    }

    void Furniture::DeleteFurniture(Collider* collider) 
    {
        if (collider->GetOwner()->GetName() == "ThrashCan")
        {
            GetOwner()->GetScene()->DeleteEntity(GetOwner());
            ThrashManager::GetInstance()->RemoveFurniture(this);
        }
    }
    #if EDITOR
    void Furniture::DrawImGui() 
    {
        ImGui::Text("Furniture Component");
    }
    #endif

    rapidjson::Value Furniture::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void Furniture::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Furniture::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // namespace Engine
