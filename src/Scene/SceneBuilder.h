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
        Build(Engine::Scene*& Scene, std::vector<unsigned int>& Textures, std::vector<Models::Model*>& Models,
              std::vector<Shaders::Shader>& Shaders, std::vector<Materials::Material*>& Materials);
    };

} // Scene
