#pragma once
#include <string>
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Prefabs/PrefabLoader.h"
namespace Engine
{
    class PrefabWindow
    {
    private:
        std::string prefabPath = "./res/prefabs/";
        std::string prefabName = "Prefab";
        std::string currentPrefabPath = "None";


    public:
        PrefabWindow() = default;
        void DrawImGui(Scene* scene);

    };
} // namespace Engine
