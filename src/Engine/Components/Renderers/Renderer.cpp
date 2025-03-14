#include "Renderer.h"
#include "Engine/EngineObjects/RenderingManager.h"

namespace Engine
{
    void Renderer::OnAdd(Engine::Entity* NewOwner)
    {
        Component::OnAdd(NewOwner);
        RenderingManager::GetInstance()->RegisterRenderer(this);
    }

    Renderer::~Renderer()
    {
        RenderingManager::GetInstance()->UnregisterRenderer(this);
    }
} // Engine
