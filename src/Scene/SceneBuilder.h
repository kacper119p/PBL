#pragma once

#include "Engine/EngineObjects/Scene/Scene.h"
#include <Engine/Components/Transform.h>
#include <Engine/EngineObjects/Entity.h>

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
