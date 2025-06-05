#pragma once

#if EDITOR

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
}

#endif EDITOR
