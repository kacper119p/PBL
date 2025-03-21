#pragma once

#include "Engine/EngineObjects/Scene.h"
#include "Materials/Material.h"
#include "Models/Model.h"
#include "Shaders/Shader.h"

namespace Scene
{
    class SceneBuilder
    {
    public:
        virtual ~SceneBuilder() = 0;

    public:
        static void
        Build(Engine::Scene*& Scene);
    };

} // Scene
