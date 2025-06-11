#include "Swapper.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Prefabs/PrefabLoader.h"
#include "Serialization/SerializationFilesUtility.h"
#include "Engine/EngineObjects/UpdateManager.h"

namespace Engine
{
    void Engine::Swapper::Start()
    {
        auto owner = this->GetOwner();
        if (!owner)
            return; 
        player = owner->GetScene()->GetPlayer();
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    void Engine::Swapper::Update(float DeltaTime)
    {
        auto defaultPlayer = static_cast<Engine::DefaultPlayer*>(player);
        Tool currentPlayerTool = defaultPlayer->GetCurrentTool();
        switch (currentPlayerTool)
        {
            case Tool::Stripper:
                if (!hasBroom)
                {
                    broom = Engine::PrefabLoader::LoadPrefab(BroomPath, this->GetOwner()->GetScene(),
                                                                             this->GetOwner()->GetTransform());
                    this->GetOwner()->GetTransform()->AddChild(broom->GetTransform());
                    hasBroom = true;
                }
                if (!hasVacuum)
                {
                    vacuum = Engine::PrefabLoader::LoadPrefab(VacuumPath, this->GetOwner()->GetScene(),
                                                                              this->GetOwner()->GetTransform());
                    this->GetOwner()->GetTransform()->AddChild(vacuum->GetTransform());
                    hasVacuum = true;
                }
                if (stripper)
                {
                    this->GetOwner()->GetTransform()->RemoveChild(stripper->GetTransform());
                    stripper->Destroy();
                }
                stripper = nullptr;
                hasStripper = false;
                break;
            case Tool::Vacuum:
                if (!hasStripper)
                {
                    stripper = Engine::PrefabLoader::LoadPrefab(
                            StripperPath, this->GetOwner()->GetScene(), this->GetOwner()->GetTransform());
                    this->GetOwner()->GetTransform()->AddChild(stripper->GetTransform());
                    hasStripper = true;
                }
                if (!hasBroom)
                {
                    broom = Engine::PrefabLoader::LoadPrefab(BroomPath, this->GetOwner()->GetScene(),
                                                                             this->GetOwner()->GetTransform());
                    this->GetOwner()->GetTransform()->AddChild(broom->GetTransform());
                    hasBroom = true;
                }
                if (vacuum)
                {
                    this->GetOwner()->GetTransform()->RemoveChild(vacuum->GetTransform());
                    vacuum->Destroy();
                }
                vacuum = nullptr;
                hasVacuum = false;
                break;
            case Tool::Broom:
                if (!hasVacuum)
                {
                    vacuum = Engine::PrefabLoader::LoadPrefab(VacuumPath, this->GetOwner()->GetScene(),
                                                                              this->GetOwner()->GetTransform());
                    this->GetOwner()->GetTransform()->AddChild(vacuum->GetTransform());
                    hasVacuum = true;
                }
                if (!hasStripper)
                {
                    stripper = Engine::PrefabLoader::LoadPrefab(
                            StripperPath, this->GetOwner()->GetScene(), this->GetOwner()->GetTransform());
                    this->GetOwner()->GetTransform()->AddChild(stripper->GetTransform());
                    hasStripper = true;
                }
                if (broom)
                {
                    this->GetOwner()->GetTransform()->RemoveChild(broom->GetTransform());
                    broom->Destroy();
                }
                broom = nullptr;
                hasBroom = false;
                break;
        }
    }

    rapidjson::Value Swapper::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void Swapper::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Swapper::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // namespace Engine
