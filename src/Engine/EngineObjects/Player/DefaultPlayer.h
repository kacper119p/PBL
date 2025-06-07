#pragma once
#include "Player.h"
#include "Engine/Components/Physics/Rigidbody.h"
#include "Engine/Components/BaseMovement/MovementComponent.h"
#include "Engine/Components/Colliders/BoxCollider.h"

namespace Engine
{
    enum Tool
    {
        Stripper = 1,
        Vacuum = 2,
        Broom = 3
    };

    class DefaultPlayer final : public Player
    {
        std::string PrefabPath = "./res/prefabs/PlayerModel.prefab";

        Rigidbody* rb = nullptr;
        MovementComponent* movementComponent = nullptr;
        BoxCollider* boxCollider = nullptr;

        Tool currentTool = Stripper;

        void Start() override;

        void Update() override;

        SERIALIZATION_EXPORT_PLAYER(DefaultPlayer)
    };
} // Engine
