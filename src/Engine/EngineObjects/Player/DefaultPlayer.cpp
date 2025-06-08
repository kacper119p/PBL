#include "DefaultPlayer.h"
#include "Engine/Prefabs/PrefabLoader.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/Transform.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Components/Game/Vacuum.h"
#include "Engine/Components/BloodSystem/BloodEraser.h"


namespace Engine
{
    void DefaultPlayer::Start()
    {
        #if !EDITOR
        UpdateManager::GetInstance()->RegisterPlayer(this);
        if (!PrefabPath.empty())
        {
            Entity* prefabEntity = PrefabLoader::LoadPrefab(PrefabPath, this->GetScene(), this->GetScene()->GetRoot()->GetTransform());
            if (prefabEntity)
            {
                this->GetTransform()->AddChild(prefabEntity->GetTransform());
            }
            rb = this->AddComponent<Engine::Rigidbody>();
            movementComponent = this->AddComponent<Engine::MovementComponent>();
            boxCollider = this->AddComponent<Engine::BoxCollider>();
            //collider settings
            boxCollider->SetHeight(2.0f);
            boxCollider->SetWidth(2.0f);
            boxCollider->SetDepth(2.0f);
            //rb settings
            rb->friction = 0.1f;
            rb->angularDamping = 0.01f;
            rb->linearDamping = 0.01f;
            rb->restitution = 0.3f;
            rb->SetMass(1.0f);
            rb->frictionEnabled = true;
            //player start position
            this->GetTransform()->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        }
        else
        {
            spdlog::warn("DefaultPlayer: Prefab path is empty, cannot load player model.");
        }
        #endif
    }

    void DefaultPlayer::Update(const float DeltaTime)
    {
        #if !EDITOR
        switch (currentTool)
        {
            case Tool::Stripper:
                if (!hasStripper)
                {
                    Entity* stripper = PrefabLoader::LoadPrefab(StripperPath, this->GetScene(), this->GetTransform());
                    this->GetTransform()->AddChild(stripper->GetTransform());
                    hasStripper = true;
                }
                hasBroom = false;
                hasVacuum = false;
                break;
            case Tool::Vacuum:
                if (!hasVacuum)
                {
                    Entity* vacuum = PrefabLoader::LoadPrefab(VacuumPath, this->GetScene(), this->GetTransform());
                    this->GetTransform()->AddChild(vacuum->GetTransform());
                    vacuum->AddComponent<Engine::Vacuum>();
                    hasVacuum = true;
                }
                hasBroom = false;
                hasStripper = false;
                break;
            case Tool::Broom:
                if (!hasBroom)
                {
                    Entity* broom = PrefabLoader::LoadPrefab(BroomPath, this->GetScene(), this->GetTransform());
                    this->GetTransform()->AddChild(broom->GetTransform());
                    hasBroom = true;
                }
                hasVacuum = false;
                hasStripper = false;
                break;
            default:
                spdlog::warn("DefaultPlayer: Unknown tool selected.");
                break;
        }
        #endif
    }
}
