#pragma once
#include <string>

#include "Scene.h"

namespace Engine
{

    class SceneManager
    {
    private:
        SceneManager();

    public:
        static void SaveScene(const std::string& Path, const Scene* Scene);
    };
} // Engine
