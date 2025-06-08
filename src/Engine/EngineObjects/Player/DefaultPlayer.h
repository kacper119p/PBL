#pragma once
#include "Player.h"
#include "Engine/Components/Physics/Rigidbody.h"
#include "Engine/Components/BaseMovement/MovementComponent.h"
#include "Engine/Components/Colliders/BoxCollider.h"
#include "Engine//Components/Game/Vacuum.h"

namespace Engine
{
    enum class Tool
    {
        Stripper = 1,
        Vacuum = 2,
        Broom = 3
    };

    class DefaultPlayer final : public Player
    {
        std::string PrefabPath = "./res/prefabs/PlayerModel.prefab";
        std::string StripperPath = "./res/prefabs/Stripper.prefab";
        std::string VacuumPath = "./res/prefabs/Vacuum.prefab";
        std::string BroomPath = "./res/prefabs/PlayerModel.prefab";

        Rigidbody* rb = nullptr;
        MovementComponent* movementComponent = nullptr;
        BoxCollider* boxCollider = nullptr;

        bool hasStripper = false;
        bool hasVacuum = false;
        bool hasBroom = false;

        Tool currentTool = Tool::Vacuum;

        void Start() override;

        void Update(float DeltaTime) override;

        SERIALIZATION_EXPORT_PLAYER(DefaultPlayer)
    };
} // Engine
