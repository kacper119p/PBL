#pragma once

#include "Engine/EngineObjects/Scene/Scene.h"

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
