#include "DefaultPlayer.h"
#include <vector>
#include "Audio/AudioManager.h"
#include "Engine/Components/BloodSystem/BloodEraser.h"
#include "Engine/Components/Game/PlayerAnimationManager.h"
#include "Engine/Components/Game/ThrashManager.h"
#include "Engine/Components/Game/Vacuum.h"
#include "Engine/Components/Transform.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Prefabs/PrefabLoader.h"
#include "Materials/VacuumIndicatorMaterial.h"
#include "spdlog/spdlog.h"


namespace Engine
{
    DefaultPlayer* DefaultPlayer::Instance = nullptr;

    void DefaultPlayer::Start()
    {
#if !EDITOR
        UpdateManager::GetInstance()->RegisterPlayer(this);
        if (!PrefabPath.empty())
        {
            Entity* prefabEntity =
                    PrefabLoader::LoadPrefab(PrefabPath, this->GetScene(), this->GetScene()->GetRoot()->GetTransform());
            if (prefabEntity)
            {
                this->GetTransform()->AddChild(prefabEntity->GetTransform());
            }
            rb = this->AddComponent<Engine::Rigidbody>();
            movementComponent = this->AddComponent<Engine::MovementComponent>();
            boxCollider = this->AddComponent<Engine::BoxCollider>();
            // collider settings
            boxCollider->SetHeight(6.0f);
            boxCollider->SetWidth(6.0f);
            boxCollider->SetDepth(6.0f);
            boxCollider->collisionMask &= ~(1 << 2);
            boxCollider->collisionLayer |= (1 << 2);
            // rb settings
            rb->friction = 0.2f;
            rb->angularDamping = 0.01f;
            rb->linearDamping = 0.01f;
            rb->restitution = 0.3f;
            rb->SetMass(1.0f);
            rb->frictionEnabled = true;
            // player start position
            this->GetTransform()->SetPosition(glm::vec3(-10.0f, 9.0f, 10.0f));

            PlayerAnimationManager* playerAnimationManager = PlayerAnimationManager::GetInstance();
            std::vector<Engine::Transform*> children = prefabEntity->GetTransform()->GetChildren();
            for (Engine::Transform* child : children)
            {
                if (child->GetOwner()->GetName() == "Track_L")
                {
                    playerAnimationManager->TrackLeft =
                            child->GetOwner()->GetComponent<Engine::AnimatedModelRenderer>();
                }
                else if (child->GetOwner()->GetName() == "Track_R")
                {
                    playerAnimationManager->TrackRight =
                            child->GetOwner()->GetComponent<Engine::AnimatedModelRenderer>();
                }
                else if (child->GetOwner()->GetName() == "HandLeft")
                {
                    playerAnimationManager->SetHandLeft(
                            child->GetOwner()->GetComponent<Engine::AnimatedModelRenderer>());
                }
                else if (child->GetOwner()->GetName() == "HandRight")
                {
                    playerAnimationManager->SetHandRight(
                            child->GetOwner()->GetComponent<Engine::AnimatedModelRenderer>());
                }
            }

            playerAnimationManager->StopAllAnimations();

            toolChangeSound = AudioManager::GetInstance().CreateSoundInstance("zmiana sprzetu");
            Instance = this;

            exhaustVfx = PrefabLoader::LoadPrefab("./res/prefabs/ExhaustVFX.prefab", this->GetScene(), this->GetTransform());
            if (exhaustVfx)
            {
                this->GetTransform()->AddChild(exhaustVfx->GetTransform());
            }

            this->GetTransform()->SetScale(glm::vec3(3.0f));
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
        AudioManager::GetInstance().SetSoundPosition(toolChangeSound, GetTransform()->GetPosition());
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
                    AudioManager::GetInstance().PlayAudio(toolChangeSound);
                }
                hasBroom = false;
                hasVacuum = false;
                if (vacuum)
                {
                    this->GetTransform()->RemoveChild(vacuum->GetTransform());
                    this->GetTransform()->RemoveChild(vacuumVfx->GetTransform());
                    PlayerAnimationManager::GetInstance()->SetVacuumVfx(nullptr);
                    PlayerAnimationManager::GetInstance()->SetVacuumShotVfx(nullptr);
                    PlayerAnimationManager::GetInstance()->StrapMaterial = nullptr;
                    vacuum->Destroy();
                }
                if (broom)
                {
                    this->GetTransform()->RemoveChild(broom->GetTransform());
                    broom->Destroy();
                }
                vacuumVfx = nullptr;
                vacuumShotVfx = nullptr;
                vacuum = nullptr;
                broom = nullptr;
                break;
            case Tool::Vacuum:
                if (!hasVacuum)
                {
                    vacuum = PrefabLoader::LoadPrefab(VacuumPath, this->GetScene(), this->GetTransform());
                    vacuumVfx = PrefabLoader::LoadPrefab("./res/prefabs/VacuumVFX.prefab", this->GetScene(),
                                                         vacuum->GetTransform());
                    vacuumShotVfx = PrefabLoader::LoadPrefab("./res/prefabs/VacuumShotVFX.prefab", this->GetScene(),
                                                             vacuum->GetTransform());
                    this->GetTransform()->AddChild(vacuum->GetTransform());
                    this->GetTransform()->AddChild(vacuumVfx->GetTransform());
                    this->GetTransform()->AddChild(vacuumShotVfx->GetTransform());
                    vacuum->AddComponent<Engine::Vacuum>();
                    hasVacuum = true;
                    PlayerAnimationManager::GetInstance()->SetVacuumVfx(vacuumVfx->GetComponent<VacuumVfx>());

                    if (!isFirstUpdate)
                    {
                        AudioManager::GetInstance().PlayAudio(toolChangeSound);
                    }

                    PlayerAnimationManager::GetInstance()->SetVacuumShotVfx(
                            vacuumShotVfx->GetComponent<VacuumShotVfx>());

                    std::vector<Engine::Transform*> children = vacuum->GetTransform()->GetChildren();
                    for (Engine::Transform* child : children)
                    {
                        if (child->GetOwner()->GetName() == "Front")
                        {
                            PlayerAnimationManager::GetInstance()->SetVacuumFront(
                                    child->GetOwner()->GetComponent<Engine::AnimatedModelRenderer>());
                        }
                        if (child->GetOwner()->GetName() == "Pasek")
                        {
                            PlayerAnimationManager::GetInstance()->StrapMaterial
                                    = dynamic_cast<Materials::VacuumIndicatorMaterial*>(
                                        child->GetOwner()->GetComponent<AnimatedModelRenderer>()->
                                        GetMaterial());
                        }
                    }
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
                    AudioManager::GetInstance().PlayAudio(toolChangeSound);
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
                    this->GetTransform()->RemoveChild(vacuumVfx->GetTransform());
                    PlayerAnimationManager::GetInstance()->SetVacuumVfx(nullptr);
                    PlayerAnimationManager::GetInstance()->SetVacuumShotVfx(nullptr);
                    PlayerAnimationManager::GetInstance()->StrapMaterial = nullptr;
                    vacuum->Destroy();
                }
                stripper = nullptr;
                vacuum = nullptr;
                vacuumVfx = nullptr;
                vacuumShotVfx = nullptr;
                break;
            default:
                spdlog::warn("DefaultPlayer: Unknown tool selected.");
                break;
        }
        if (isFirstUpdate)
            isFirstUpdate = false;
#endif
    }

    void DefaultPlayer::SetTool(Tool tool) { this->currentTool = tool; }

} // namespace Engine
