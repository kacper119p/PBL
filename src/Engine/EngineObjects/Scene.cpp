//
// Created by Kacper on 17.11.2024.
//

#include "Scene.h"

namespace Engine
{
    Scene::Scene()
    {
        Root = new Entity();
    }

    Scene::Scene(Entity* Root) :
        Root(Root)
    {
    }

    Scene::~Scene() = default;
} // Engine
