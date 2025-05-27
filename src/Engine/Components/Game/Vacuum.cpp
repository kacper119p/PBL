#include "Vacuum.h"
#include "Engine/EngineObjects/Entity.h"
#include "Thrash.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Components/Physics/Rigidbody.h"
#include "Engine/EngineObjects/Scene/Scene.h"

void Vacuum::Start() 
{ 
	collider = this->GetOwner()->AddComponent<Engine::SphereCollider>();
    collider->SetTrigger(true);
    Engine::UpdateManager::GetInstance()->RegisterComponent(this);
}

void Vacuum::Update(float deltaTime) 
{
    if (volume<=maxVolume)
    {
        auto position = this->GetOwner()->GetTransform()->GetPosition();
        std::vector<Engine::Collider*> entities = collider->SphereOverlap(position, size);

        for (int i = 0; i < entities.size(); i++)
        {
            if (entities[i]->GetOwner()->GetComponent<Thrash>())
            {
                glm::vec3 direction = position - entities[i]->GetOwner()->GetTransform()->GetPosition();
                entities[i]->GetOwner()->GetComponent<Engine::Rigidbody>()->AddForce(direction,
                                                                                     Engine::ForceMode::Force);
            }
        }

        entities = collider->SphereOverlap(position, centerSize);
        for (int i = 0; i < entities.size(); i++)
        {
            if (entities[i]->GetOwner()->GetComponent<Thrash>())
            {
                auto thrashSize = entities[i]->GetOwner()->GetComponent<Thrash>()->GetSize();
                int thrashSizeInt = static_cast<int>(entities[i]->GetOwner()->GetComponent<Thrash>()->GetSize());
                if (volume+thrashSizeInt <= maxVolume)
                {
                    items.push_back(entities[i]->GetOwner());
                    volume += thrashSizeInt;
                    entities[i]->GetOwner()->GetScene()->DeleteEntity(entities[i]->GetOwner());
                }
            }
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