#include "Renderer.h"
#include "Engine/EngineObjects/RenderingManager.h"

namespace Engine
{
    void Renderer::Start()
    {
        Component::Start();
        RenderingManager::GetInstance()->RegisterRenderer(this);
    }

    Renderer::~Renderer()
    {
        RenderingManager::GetInstance()->UnregisterRenderer(this);
    }
} // Engine
