#pragma once

#include "Engine/Components/Game/Furniture.h"
#include <vector>
namespace Engine
{
    class Thrash;
    class ThrashManager
    {
    private:
        static inline ThrashManager* Instance{nullptr};

        std::vector<Thrash*> Thrashes;
        std::vector<Furniture*> Furnitures;


    public:
        ThrashManager() = default;
        ~ThrashManager() = default;
        static ThrashManager* GetInstance();
        void AddThrash(Thrash* thrash) { Thrashes.push_back(thrash); }
        void AddFurniture(Furniture* furniture) { Furnitures.push_back(furniture); }
        void RemoveThrash(Thrash* thrash);
        void RemoveFurniture(Furniture* furniture);

        int GetThrashCount() { return Thrashes.size(); }
        int GetFurnitureCount() { return Furnitures.size(); }

        int VacuumCount;
        int VacuumVolume;
    };
} // namespace Engine
