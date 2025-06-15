#pragma once
#include <string>

#include "Engine/EngineObjects/Scene/Scene.h"

namespace Engine
{

    class PrefabLoader
    {
    private:
        PrefabLoader() = default;

    public:
        static Entity* LoadPrefab(const std::string& Path, Scene* Scene, Transform* Parent);

        static void SavePrefabToFile(const std::string& Path, const Entity* Entity);
    };

}
