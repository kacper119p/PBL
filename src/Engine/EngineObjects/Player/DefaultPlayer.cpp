#include "DefaultPlayer.h"
#include "Engine/Prefabs/PrefabLoader.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/Transform.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Components/Game/Vacuum.h"
#include "Engine/Components/BloodSystem/BloodEraser.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Components/Game/ThrashManager.h"


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
            boxCollider->OnCollisionAddListener(SwapTool);
            //rb settings
            rb->friction = 0.05f;
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
        InputManager& input = InputManager::GetInstance();
        if (input.IsKeyPressed(GLFW_KEY_E) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_X))
        {
            canSwap = true;
        }
        else
        {
            canSwap = false;
        }
        switch (currentTool)
        {
            case Tool::Stripper:
                if (!hasStripper)
                {
                    stripper = PrefabLoader::LoadPrefab(StripperPath, this->GetScene(), this->GetTransform());
                    this->GetTransform()->AddChild(stripper->GetTransform());
                    hasStripper = true;
                }
                hasBroom = false;
                hasVacuum = false;
                if (vacuum)
                {
                    this->GetTransform()->RemoveChild(vacuum->GetTransform());
                    vacuum->Destroy();
                }
                if (broom)
                {
                    this->GetTransform()->RemoveChild(broom->GetTransform());
                    broom->Destroy();
                }
                vacuum = nullptr;
                broom = nullptr;
                break;
            case Tool::Vacuum:
                if (!hasVacuum)
                {
                    vacuum = PrefabLoader::LoadPrefab(VacuumPath, this->GetScene(), this->GetTransform());
                    this->GetTransform()->AddChild(vacuum->GetTransform());
                    vacuum->AddComponent<Engine::Vacuum>();
                    hasVacuum = true;
                }
                hasBroom = false;
                hasStripper = false;
                if (broom)
                {
                    this->GetTransform()->RemoveChild(broom->GetTransform());
                    broom->Destroy();
                }
                if (stripper)
                {
                    this->GetTransform()->RemoveChild(stripper->GetTransform());
                    stripper->Destroy();
                }
                stripper = nullptr;
                broom = nullptr;
                break;
            case Tool::Broom:
                if (!hasBroom)
                {
                    broom = PrefabLoader::LoadPrefab(BroomPath, this->GetScene(), this->GetTransform());
                    this->GetTransform()->AddChild(broom->GetTransform());
                    hasBroom = true;
                }
                hasVacuum = false;
                hasStripper = false;
                if (stripper)
                {
                    this->GetTransform()->RemoveChild(stripper->GetTransform());
                    stripper->Destroy();
                }
                if (vacuum)
                {
                    this->GetTransform()->RemoveChild(vacuum->GetTransform());
                    vacuum->Destroy();
                }
                stripper = nullptr;
                vacuum = nullptr;
                break;
            default:
                spdlog::warn("DefaultPlayer: Unknown tool selected.");
                break;
        }
        #endif
    }
    void DefaultPlayer::SetTool(Tool tool) 
    { 
        this->currentTool = tool; 
    }

    void DefaultPlayer::ToolSwapper(Collider* collider) 
    {
        if (canSwap)
        {
            if (collider->GetOwner()->GetName() == "StripperCollider")
            {
                SetTool(Tool::Stripper);
            }
            else if (collider->GetOwner()->GetName() == "VacuumCollider")
            {
                SetTool(Tool::Vacuum);
            }
            else if (collider->GetOwner()->GetName() == "BroomCollider")
            {
                SetTool(Tool::Broom);
            }
        }
    }
}
