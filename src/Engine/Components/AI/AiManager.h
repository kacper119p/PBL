#pragma once

#include "AStar.h"
#include "BehaviorTreeNode.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Components/Updateable.h"

namespace Engine
{
    class AiManager : public IUpdateable, public Component
    {
    public:
        AiManager();

        ~AiManager();

        void Start() override;

        void Update(float DeltaTime) override;

        void StartChase();

        void StopChase();

        [[nodiscard]] float GetChaseTimer() const { return ChaseTimer; }
        [[nodiscard]] bool IsChasingPlayer() const { return IsChasing; }
        [[nodiscard]] AStar* GetAStar() const { return AStarComponent; }
        [[nodiscard]] Entity* GetPlayer() const { return Player; }
        [[nodiscard]] float GetPlayerRange() const { return PlayerRange; }
        [[nodiscard]] float GetFastMovementSpeed() const { return FastMovementSpeed; }
        [[nodiscard]] float GetSlowMovementSpeed() const { return SlowMovementSpeed; }
        [[nodiscard]] float GetChaseCooldown() const { return ChaseCooldown; }
        [[nodiscard]] float GetRestTimer() const { return RestTimer; }
        [[nodiscard]] float GetRestCooldown() const { return RestCooldown; }
        [[nodiscard]] bool IsRestFinished() const { return RestFinished; }

        [[nodiscard]] bool IsTrashNearby() const;

        [[nodiscard]] bool IsChaseTimerOver() const;

        [[nodiscard]] bool IsPlayerInRange() const;

        void SetIsChasingPlayer(bool Value) { IsChasing = Value; }
        void SetRestFinished(bool Value) { RestFinished = Value; }

        void RecalculateCurrentTrash();

    private:
        void InitializeBehaviorTree();

        void InitPlayer();

#if EDITOR
        void AddEntityRecursivelyToTrash(Entity* entity);

        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(AiManager)

        AStar* AStarComponent = nullptr;
        std::shared_ptr<BehaviorTreeNode> RootBehavior;

        float ChaseTimer = 0.0f;
        bool IsChasing = false;
        float ChaseCooldown = 5.0f;
        Entity* Player = nullptr;
        float RestTimer = 0.0f;
        float RestCooldown = 3.0f;
        bool IsResting = false;
        float PlayerRange = 7.0f;
        float FastMovementSpeed = 8.0f;
        float SlowMovementSpeed = 3.0f;
        bool RestFinished = false;
        std::vector<Entity*> TrashEntities;
        float TrashRange = 3.0f;
        Entity* TargetTrash = nullptr;
        int CurrentTrashValue = 0;
        int MaxTrashCapacity = 10;
        std::string SelectedPlayerName = "";

    };
}
