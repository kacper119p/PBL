#include "StartGameButton.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "Engine/Components/Game/ThrashManager.h"
namespace Engine
{
       
    void StartGameButton::OnStartGame(Collider* collider) 
    { 
        //TODO: ustawic tu 1 lvl jak juz bedzie
        SceneManager::ChangeScene("./res/scenes/Gameplay.lvl");
        ThrashManager::GetInstance()->SetCurrentLevel(1);
    }
    void StartGameButton::Start() 
    {
        #if !EDITOR
        collider = this->GetOwner()->GetComponent<BoxCollider>();
        if (collider != nullptr)
        {
            collider->OnCollisionAddListener(StartGameAction);
        }
        #endif
    }
    rapidjson::Value StartGameButton::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void StartGameButton::DeserializeValuePass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void StartGameButton::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                    Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // namespace Engine
