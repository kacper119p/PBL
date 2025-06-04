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
        static void LoadPrefab(const std::string& Path, Scene* Scene);

        static void SavePrefabToFile(const std::string& Path, const Entity* Entity);
    };

}
