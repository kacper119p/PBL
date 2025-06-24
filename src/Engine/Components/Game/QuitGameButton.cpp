#include "QuitGameButton.h"
#include "Engine/EngineObjects/WindowManager.h"
#include "Engine/EngineObjects/Entity.h"
namespace Engine
{
    void QuitGameButton::OnQuitGame(Collider* collider) 
    { 
        WindowManager::GetInstance()->QuitGame(); }

    void QuitGameButton::Start()
    {
#if !EDITOR 
        collider = this->GetOwner()->GetComponent<BoxCollider>();
        if (collider != nullptr)
        {
            collider->OnCollisionAddListener(QuitGameAction);
        }
#endif 
    }

    rapidjson::Value QuitGameButton::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void QuitGameButton::DeserializeValuePass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void QuitGameButton::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                    Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // namespace Engine
