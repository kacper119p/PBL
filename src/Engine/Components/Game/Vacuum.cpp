#include "Vacuum.h"
#include "Engine/EngineObjects/Entity.h"
#include "Thrash.h"
#include "Engine/EngineObjects/UpdateManager.h"

void Vacuum::Start() 
{ 
	collider = this->GetOwner()->AddComponent<Engine::SphereCollider>();
    collider->SetTrigger(true);
    Engine::UpdateManager::GetInstance()->RegisterComponent(this);
}

void Vacuum::Update(float deltaTime) 
{
    auto position = this->GetOwner()->GetTransform()->GetPosition();
    std::vector<Engine::Collider*> entities = collider->SphereOverlap(position, size);

    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i]->GetOwner()->GetComponent<Thrash>())
        {
            continue;
        }
    }
}
rapidjson::Value Vacuum::Serialize(rapidjson::Document::AllocatorType& Allocator) const
{
    START_COMPONENT_SERIALIZATION
    END_COMPONENT_SERIALIZATION
}

void Vacuum::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_VALUE_PASS
    END_COMPONENT_DESERIALIZATION_VALUE_PASS
}

void Vacuum::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
{
    // Implementacja
}
#if EDITOR
void Vacuum::DrawImGui() {}
#endif