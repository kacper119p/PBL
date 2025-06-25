#pragma once
#include "../Component.h"
#include "Engine//EngineObjects/Player/DefaultPlayer.h"
#include "Serialization/SerializationUtility.h"
#include "Events/TAction.h"

namespace Engine
{
    class Swapper : public Updateable
    {
    private:
        std::string StripperPath = "./res/prefabs/StripperSwapper.prefab";
        std::string VacuumPath = "./res/prefabs/VacuumSwapper.prefab";
        std::string BroomPath = "./res/prefabs/BroomSwapper.prefab";

        Events::TAction<Collider*> SwapToolStripper =Events::TAction<Collider*>(this, &Swapper::SwapPlayerToolStripper);
        Events::TAction<Collider*> SwapToolBroom =Events::TAction<Collider*>(this, &Swapper::SwapPlayerToolBroom);
        Events::TAction<Collider*> SwapToolVacuum =Events::TAction<Collider*>(this, &Swapper::SwapPlayerToolVacuum);

        Player* player = nullptr;
        public: 
        Swapper() = default;
        ~Swapper() override = default;
        void Start() override;

        bool hasStripper = false;
        bool hasVacuum = false;
        bool hasBroom = false;

        Collider* stripperCollider = nullptr;
        Collider* vacuumCollider = nullptr;
        Collider* broomCollider = nullptr;


        Entity* stripper = nullptr;
        Entity* vacuum = nullptr;
        Entity* broom = nullptr;

        void Update(float DeltaTime) override;

        void SwapPlayerToolStripper(Collider* collider);
        void SwapPlayerToolBroom(Collider* collider);
        void SwapPlayerToolVacuum(Collider* collider);

    public:

        SERIALIZATION_EXPORT_CLASS(Swapper);
#if EDITOR
        void DrawImGui() override {};
#endif
    };
} // namespace Engine
