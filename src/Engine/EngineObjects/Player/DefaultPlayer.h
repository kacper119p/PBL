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
        std::string BroomPath = "./res/prefabs/Broom.prefab";

        Events::TAction<Collider*> SwapTool = Events::TAction<Collider*>(this, &DefaultPlayer::ToolSwapper);

        Rigidbody* rb = nullptr;
        MovementComponent* movementComponent = nullptr;
        BoxCollider* boxCollider = nullptr;

        Entity* stripper = nullptr;
        Entity* vacuum = nullptr;
        Entity* broom = nullptr;

        bool hasStripper = false;
        bool hasVacuum = false;
        bool hasBroom = false;

        bool canSwap = false;

        Tool currentTool = Tool::Broom;

        void Start() override;

        void Update(float DeltaTime) override;

        public:
        void SetTool(Tool tool);
        Tool GetCurrentTool() const { return currentTool; }

        void ToolSwapper(Collider* collider);

        SERIALIZATION_EXPORT_PLAYER(DefaultPlayer)
    };
} // Engine
