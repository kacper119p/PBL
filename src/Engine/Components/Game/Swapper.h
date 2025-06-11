#pragma once
#include "../Component.h"
#include "Engine//EngineObjects/Player/DefaultPlayer.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    class Swapper : public Updateable
    {
    private:
        std::string StripperPath = "./res/prefabs/StripperSwapper.prefab";
        std::string VacuumPath = "./res/prefabs/VacuumSwapper.prefab";
        std::string BroomPath = "./res/prefabs/BroomSwapper.prefab";

        Player* player = nullptr;
        public: 
        Swapper() = default;
        ~Swapper() override = default;
        void Start() override;

        bool hasStripper = false;
        bool hasVacuum = false;
        bool hasBroom = false;

        Entity* stripper = nullptr;
        Entity* vacuum = nullptr;
        Entity* broom = nullptr;

        void Update(float DeltaTime) override;

    public:

        SERIALIZATION_EXPORT_CLASS(Swapper);
#if EDITOR
        void DrawImGui() override {};
#endif
    };
} // namespace Engine
